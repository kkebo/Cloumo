#include "../headers.h"
#include <setjmp.h>
#include <string.h>

using TekTPrb = unsigned int;

static int TekDecode5(int siz, unsigned char *p, unsigned char *q);

static unsigned int Tekgetnum_s7s(unsigned char **pp) {
/* これは必ずbig-endian */
/* 下駄がないので中身をいじりやすい */
	unsigned int s = 0;
	unsigned char *p = *pp;
	do {
		s = s << 7 | *p++;
	} while ((s & 1) == 0);
	s >>= 1;
	*pp = p;
	return s;
}

int TekGetSize(unsigned char *p) {
	static char header[15] = {
		static_cast<char>(0xff),
		static_cast<char>(0xff),
		static_cast<char>(0xff),
		static_cast<char>(0x01),
		static_cast<char>(0x00),
		static_cast<char>(0x00),
		static_cast<char>(0x00),
		static_cast<char>(0x4f),
		static_cast<char>(0x53),
		static_cast<char>(0x41),
		static_cast<char>(0x53),
		static_cast<char>(0x4b),
		static_cast<char>(0x43),
		static_cast<char>(0x4d),
		static_cast<char>(0x50)
	};
	if (memcmp(p + 1, header, 15) == 0 && (*p == 0x83 || *p == 0x85 || *p == 0x89)) {
		return Tekgetnum_s7s(&(p += 16));
	}
	return -1;
}	  /* （註）memcmpはstrncmpの仲間で、文字列中に0があっても指定された15文字まで比較する関数 */

int TekDecomp(unsigned char *p, unsigned char *q, int size) {
	if (*p == 0x89 && TekDecode5(size, p, q) != 0) return -1;
	return 0;	/* 成功 */
}

static int TekDecMain5(int *work, unsigned char *src, int osiz, unsigned char *q, int lc, int pb, int lp, int flags);

static int TekLzRestoreTek5(int srcsiz, unsigned char *src, int outsiz, unsigned char *outbuf)
{
	int wrksiz, lc, lp, pb, flags = 0, *work, prop0, fl;

	if ((fl = (prop0 = *src) & 0x0f) == 0x01) /* 0001 */
		flags |= -1;
	else if (fl == 0x05)
		flags = -2;
	else if (fl == 0x09)
		flags &= 0;
	else
		return 1;
	src++;
	prop0 >>= 4;
	if (prop0 == 0)
		prop0 = *src++;
	else {
		static unsigned char prop0_table[] = { 0x5d, 0x00 }, prop1_table[] = { 0x00 };
		if (flags == -1) {
			if (prop0 >= 3)
				return 1;
			prop0 = prop0_table[prop0 - 1];
		} else {
			if (prop0 >= 2)
				return 1;
			prop0 = prop1_table[prop0 - 1];
		}
	}
	lp = prop0 / (9 * 5);
	prop0 %= 9 * 5;
	pb = prop0 / 9;
	lc = prop0 % 9;
	if (flags == 0) /* tek5:z2 */
		flags = *src++;
	if (flags == -1) { /* stk5 */
		wrksiz = lp;
		lp = pb;
		pb = wrksiz;
	}
	wrksiz = 0x180 * sizeof(unsigned int) + (0x840 + (0x300 << (lc + lp))) * sizeof(TekTPrb); /* 最低15KB, lc+lp=3なら、36KB */
	work = (int *)malloc4k(wrksiz);
	if (work == nullptr) {
		return -1;
	}
	flags = TekDecMain5(work, src, outsiz, outbuf, lc, pb, lp, flags);
	free4k(work);
	return flags;
}

struct TekStrBitModel {
	unsigned char t, m, s, dmy;
	unsigned int prb0, prb1, tmsk, ntm, lt, lt0, dmy4;
};

struct TekStrPrb {
	struct TekStrPrbPb {
		struct TekStrPrbPbSt {
			TekTPrb mch, rep0l1;
		} st[12];
		TekTPrb lenlow[2][8], lenmid[2][8];
	} pb[16];
	struct TekStrPrbSt {
		TekTPrb rep, repg0, repg1, repg2;
	} st[12];
	TekTPrb lensel[2][2], lenhigh[2][256], pslot[4][64], algn[64];
	TekTPrb spdis[2][2+4+8+16+32], lenext[2+4+8+16+32];
	TekTPrb repg3, fchgprm[2 * 32], tbmt[16], tbmm[16], fchglt;
	TekTPrb lit[1];
};

struct TekStrRngDec {
	unsigned char *p;
	unsigned int range, code, rmsk;
	//jmp_buf errjmp; original
	void **errjmp;
	TekStrBitModel bm[32], *ptbm[16];
	TekStrPrb probs;
};

static void TekSetBM5(TekStrBitModel *bm, int t, int m) {
	bm->t = t;
	bm->m = m;
	bm->prb1 = -1 << (m + t);
	bm->prb0 = ~bm->prb1;
	bm->prb1 |= 1 << t;
	bm->tmsk = (-1 << t) & 0xffff;
	bm->prb0 &= bm->tmsk;
	bm->prb1 &= bm->tmsk;
	bm->ntm = ~bm->tmsk;
	return;
}

static int TekRDGet0(TekStrRngDec *rd, int n, int i) {
	do {
		while (rd->range < (unsigned int) (1 << 24)) {
			rd->range <<= 8;
			rd->code = rd->code << 8 | *rd->p++;
		}
		rd->range >>= 1;
		i += i;
		if (rd->code >= rd->range) {
			rd->code -= rd->range;
			i |= 1;
		}
	} while (--n);
	return ~i;
}

static int TekRDGet1(struct TekStrRngDec *rd, TekTPrb *prob0, int n, int j, struct TekStrBitModel *bm) {
	unsigned int p, i, *prob, nm = n >> 4;
	n &= 0x0f;
	prob0 -= j;
	do {
		p = *(prob = prob0 + j);
		if (bm->lt > 0) {
			if (--bm->lt == 0) {
				/* 寿命切れ */
				if (TekRDGet1(rd, &rd->probs.fchglt, 0x71, 0, &rd->bm[3]) == 0) {
					/* 寿命変更はまだサポートしてない */
err:
					longjmp(rd->errjmp, 1);
				}
				i = bm - rd->bm;
				if ((bm->s = TekRDGet1(rd, &rd->probs.fchgprm[i * 2 + bm->s], 0x71, 0, &rd->bm[1])) == 0) {
					i = TekRDGet1(rd, rd->probs.tbmt, 0x74, 1, &rd->bm[2]) & 15;
					if (i == 15)
						goto err;
					TekSetBM5(bm, i, ((TekRDGet1(rd, rd->probs.tbmm, 0x74, 1, &rd->bm[2]) - 1) & 15) + 1);
				}
				bm->lt = bm->lt0;
			}
			if (p < bm->prb0) {
				p = bm->prb0;
				goto fixprob;
			}
			if (p > bm->prb1) {
				p = bm->prb1;
				goto fixprob;
			}
			if (p & bm->ntm) {
				p &= bm->tmsk;
	fixprob:
				*prob = p;
			}
		}

		while (rd->range < (unsigned int) (1 << 24)) {
			rd->range <<= 8;
			rd->code = rd->code << 8 | *rd->p++;
		}
		j += j;
		i = ((unsigned long long) (rd->range & rd->rmsk) * p) >> 16;
		if (rd->code < i) {
			j |= 1;
			rd->range = i;
			*prob += ((0x10000 - p) >> bm->m) & bm->tmsk;
		} else {
			rd->range -= i;
			rd->code -= i;
			*prob -= (p >> bm->m) & bm->tmsk;
		}
		--n;
		if ((n & nm) == 0)
			bm++;
	} while (n);
	return j;
}

static unsigned int TekRevBit(unsigned int data, int len) {
	unsigned int rev = 0;
	do {
		rev += rev + (data & 1);
		data >>= 1;
	} while (--len);
	return rev;
}

static int TekGetLen5(struct TekStrRngDec *rd, int m, int s_pos, int stk) {
	int i;
	if (TekRDGet1(rd, &rd->probs.lensel[m][0], 0x71, 0, rd->ptbm[3]) ^ stk) { /* low */
		i = TekRDGet1(rd, rd->probs.pb[s_pos].lenlow[m], 0x73, 1, rd->ptbm[4]) & 7;
	} else if (TekRDGet1(rd, &rd->probs.lensel[m][1], 0x71, 0, rd->ptbm[3]) ^ stk) { /* mid */
		i = TekRDGet1(rd, rd->probs.pb[s_pos].lenmid[m], 0x73, 1, rd->ptbm[5]);
	} else {
		/* high */
		i = TekRDGet1(rd, rd->probs.lenhigh[m], 0x78, 1, rd->ptbm[6]) - (256 + 256 - 8);
		if (i > 0) {
			if (i < 6 && stk == 0) {
				i = TekRDGet1(rd, &rd->probs.lenext[(1 << i) - 2], i | 0x70, 1, rd->ptbm[7]) - 1;
			} else {
				i = TekRDGet0(rd, i, ~1) - 1;
			}
			i = TekRDGet0(rd, i, ~1) - 1;
		}
		i += 256 - 8 + 16;
	}
	return i;
}

static int TekDecMain5(int *work, unsigned char *src, int osiz, unsigned char *q, int lc, int pb, int lp, int flags) {
	static int state_table[] = { 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 4, 5 };
	int i, j, k, pmch, rep[4], s, pos, m_pos = (1 << pb) - 1, m_lp = (1 << lp) - 1;
	int stk = (flags == -1), lcr = 8 - lc, s_pos, lit0cntmsk = 0x78;
	unsigned int *lit1;
	TekStrRngDec *rd = (TekStrRngDec*)work;
	TekStrPrb *prb = &rd->probs;

	rd->p = &src[4];
	rd->range |= -1;
	rd->code = src[0] << 24 | src[1] << 16 | src[2] << 8 | src[3];
	for (i = 0; i < 4; ++i) {
		rep[i] = ~i;
	}
	if (setjmp(rd->errjmp)) {
		goto err;
	}
	for (i = sizeof(TekStrPrb) / sizeof(TekTPrb) + (0x300 << (lc + lp)) - 2; i >= 0; --i) {
		((TekTPrb*)prb)[i] = 1 << 15;
	}
	for (i = 0; i < 32; ++i) {
		rd->bm[i].lt = (i >= 4); /* 0..3は寿命なし */
		rd->bm[i].lt0 = (i < 24) ? 16 * 1024 : 8 * 1024;
		rd->bm[i].s &= 0;
		rd->bm[i].t = rd->bm[i].m = 5;
	}
	lit1 = prb->lit + ((256 << (lc + lp)) - 2);
	if (stk) {
		rd->rmsk = -1 << 11;
		for (auto &&bm : rd->bm)
			bm.lt = 0; /* 全て寿命なし */
		for (auto &&ptbm : rd->ptbm)
			ptbm = &rd->bm[0];
	} else {
		unsigned char pt[14];
		static unsigned char pt1[14] = {
			8,  8,  8,  8,  8,  8,  8,  8,
			8,  8, 18, 18, 18,  8
		};
		static unsigned char pt2[14] = {
			8,  8, 10, 11, 12, 12, 14, 15,
			16, 16, 18, 18, 20, 21
		};
		/*
			0- 7:mch, mch, lit1, lensel, lenlow, lenmid, lenhigh, lenext
			8-15:pslot, pslot, sdis, sdis, align, rep-repg2
		*/
		rd->rmsk |= -1;
		rd->bm[1].t = 5; rd->bm[1].m = 3; /* for fchgprm */
		rd->bm[2].t = 9; rd->bm[2].m = 2; /* for tbmt, tbmm */
		if (flags & 0x40) { /* lt-flag */
			rd->bm[3].t = 0; rd->bm[3].m = 1;
			prb->fchglt = 0xffff;
		}
		rd->bm[22].t = 0; rd->bm[22].m = 1;
		prb->repg3 = 0xffff;
		if (flags == -2) { /* z1 */
			rd->bm[22].lt = 0; /* repg3のltを0に */
			memcpy(pt, pt1, sizeof pt1);
		} else {
			memcpy(pt, pt2, sizeof pt2);
			lit0cntmsk = (7 >> (flags & 3)) << 4 | 8;
			pt[ 1] =  8 + ((flags & 0x04) != 0); /* mch */
			pt[ 5] = 12 + ((flags & 0x08) != 0); /* llm */
			pt[ 9] = 16 + ((flags & 0x10) != 0); /* pst */
			pt[11] = 18 + ((flags & 0x20) != 0); /* sds */
		}
		for (i = 0; i < 14; ++i) {
			rd->ptbm[i] = &rd->bm[pt[i]];
		}
	}
	for (auto &&bm : rd->bm) {
		TekSetBM5(&bm, bm.t, bm.m);
	}

	if ((TekRDGet1(rd, &prb->pb[0].st[0].mch, 0x71, 0, rd->ptbm[0]) ^ stk) == 0) {
		goto err;
	}
	*q++ = TekRDGet1(rd, prb->lit, lit0cntmsk, 1, &rd->bm[24]) & 0xff;
	pmch &= 0; s &= 0; pos = 1;
	while (pos < osiz) {
		s_pos = pos & m_pos;
		if (TekRDGet1(rd, &prb->pb[s_pos].st[s].mch, 0x71, 0, rd->ptbm[s > 0]) ^ stk) { /* 非lz */
			i = (q[-1] >> lcr | (pos & m_lp) << lc) << 8;
			s = state_table[s];
			if (pmch == 0)
				*q = TekRDGet1(rd, &prb->lit[i], lit0cntmsk, 1, &rd->bm[24]) & 0xff;
			else {
				struct TekStrBitModel *bm = &rd->bm[24];
				j = 1; /* lit1は最初から2を減じてある */
				k = 8;
				pmch = q[rep[0]];
				do {
					j += j + TekRDGet1(rd, &lit1[(i + j) << 1 | pmch >> 7], 0x71, 0, rd->ptbm[2]);
					--k;
					if ((k & (lit0cntmsk >> 4)) == 0)
						bm++;
					if ((((pmch >> 7) ^ j) & 1) != 0 && k != 0) {
						j = TekRDGet1(rd, &prb->lit[i + j - 1], k | (lit0cntmsk & 0x70), j, bm);
						break;
					}
					pmch <<= 1;
				} while (k);
				*q = j & 0xff;
				pmch &= 0;
			}
			pos++;
			q++;
		} else { /* lz */
			pmch |= 1;
			if (TekRDGet1(rd, &prb->st[s].rep, 0x71, 0, rd->ptbm[13]) ^ stk) { /* len/dis */
				rep[3] = rep[2];
				rep[2] = rep[1];
				rep[1] = rep[0];
				j = i = TekGetLen5(rd, 0, s_pos, stk);
				s = s < 7 ? 7 : 10;
				if (j >= 4)
					j = 3;
				rep[0] = j = TekRDGet1(rd, prb->pslot[j], 0x76, 1, rd->ptbm[8 + (j == 3)]) & 0x3f;
				if (j >= 4) {
					k = (j >> 1) - 1; /* k = [1, 30] */
					rep[0] = (2 | (j & 1)) << k;
					if (j < 14) /* k < 6 */
						rep[0] |= TekRevBit(TekRDGet1(rd, &prb->spdis[j & 1][(1 << k) - 2], k | 0x70, 1, rd->ptbm[10 + (k >= 4)]), k);
					else {
						if (stk == 0) {
							if (k -= 6)
								rep[0] |= TekRDGet0(rd, k, ~0) << 6;
							rep[0] |= TekRevBit(TekRDGet1(rd, prb->algn, 0x76, 1, rd->ptbm[12]), 6);
						} else {
							rep[0] |= TekRDGet0(rd, k - 4, ~0) << 4;
							rep[0] |= TekRevBit(TekRDGet1(rd, prb->algn, 0x74, 1, rd->ptbm[12]), 4);
						}
					}
				}
				rep[0] = ~rep[0];
			} else { /* repeat-dis */
				if (TekRDGet1(rd, &prb->st[s].repg0, 0x71, 0, rd->ptbm[13]) ^ stk) { /* rep0 */
					i |= -1;
					if (TekRDGet1(rd, &prb->pb[s_pos].st[s].rep0l1, 0x71, 0, rd->ptbm[13]) == 0) {
						s = s < 7 ? 9 : 11;
						goto skip;
					}
				} else {
					if (TekRDGet1(rd, &prb->st[s].repg1, 0x71, 0, rd->ptbm[13]) ^ stk) /* rep1 */
						i = rep[1];
					else {
						if (TekRDGet1(rd, &prb->st[s].repg2, 0x71, 0, rd->ptbm[13]) ^ stk) /* rep2 */
							i = rep[2];
						else {
							if (stk == 0) {
								if  (TekRDGet1(rd, &prb->repg3, 0x71, 0, &rd->bm[22]) == 0)
									goto err;
							}
							i = rep[3]; /* rep3 */
							rep[3] = rep[2];
						}
						rep[2] = rep[1];
					}
					rep[1] = rep[0];
					rep[0] = i;
				}
				i = TekGetLen5(rd, 1, s_pos, stk);
				s = s < 7 ? 8 : 11;
			}
skip:
			i += 2;
			if (pos + rep[0] < 0)
				goto err;
			if (pos + i > osiz)
				i = osiz - pos;
			pos += i;
			do {
				*q = q[rep[0]];
				q++;
			} while (--i);
		}
	}
	return 0;
err:
	return 1;
}

static int TekDecode5(int siz, unsigned char *p, unsigned char *q) {
	unsigned char *p1 = p + siz;
	int dsiz, hed, bsiz, st = 0;
	p += 16;
	if ((dsiz = Tekgetnum_s7s(&p)) > 0) {
		hed = Tekgetnum_s7s(&p);
		if ((hed & 1) == 0)
			st = TekLzRestoreTek5(p1 - p + 1, p - 1, dsiz, q);
		else {
			bsiz = 1 << (((hed >> 1) & 0x0f) + 8);
			if (hed & 0x20)
				return 1;
			if (bsiz == 256)
				st = TekLzRestoreTek5(p1 - p, p, dsiz, q);
			else {
				if (dsiz > bsiz)
					return 1;
				if (hed & 0x40)
					Tekgetnum_s7s(&p); /* オプション情報へのポインタを読み飛ばす */
				st = TekLzRestoreTek5(p1 - p, p, dsiz, q);
			}
		}
	}
	return st;
}
