/*============================================================================
  旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커
  � MSKCHK.C   (C) Copyright 1992 Bruce E. H봥man.  All Rights Reserved. �
  읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸
  MSKCHK - Function to scan a given area for a match against a mask string
  containing wild cards and constant characters.
  This is an extension of the usual DOS wild-card filespec matching, as it
  permits imbedded '*' and '?' and the mask is an arbitrary length.

  Input:  mask, mask_len, area, area_len, qmark, asterisk
          'qmark' is single-wild-card char, 'asterisk' is 0 to n wild chars.
============================================================================*/
#include <stddef.h>
#include <stdio.h>
#define ERROR  -1
#define WARNING 4
#define DONE   0x20 /* Whole mask is complete */
#define MATCH  0x10 /* Whole mask is found/matched */
#define FAIL   0x08 /* Whole mask is not found */
#define FLTFND 0x04 /* Mask float substr found */
#ifndef FALSE
#define FALSE  0
#endif
#ifndef TRUE
#define TRUE  ~FALSE
#endif
  unsigned long irc;
  unsigned long iflg;
  unsigned long iMaskOffset;
  unsigned long iAreaOffset;
  char *cMask;
  unsigned long iMaskLen;
  char *cArea;
  unsigned long iAreaLen;
  char *cQ;
  char *cA;

/*=============
  AString
  Current mask character is "*" wild card.
  (For "?" that appear after initial "*", process these.)
  We must scan mask string for its terminator.
=============*/
  static void AString (void)
{ unsigned long ixMaskOffset,      /* ptr '*' 2 in      *XXX* */
      iyMaskOffset;
  unsigned long ixAreaOffset, iyAreaOffset;
  unsigned long iMatchCount, iwMaskLen;
  while (TRUE)                     /* Process '*' and '?' at start */
  { if (iflg & DONE) break;

    if ((*cA != *(cMask+iMaskOffset))
    &&  (*cQ != *(cMask+iMaskOffset))) break;

    while (*cA == *(cMask+iMaskOffset))
    { iMaskOffset++;
      if (iMaskOffset >= iMaskLen) { iflg |= DONE+MATCH; break;}
    }
    while (*cQ == *(cMask+iMaskOffset))
    { if (iAreaOffset >= iAreaLen) { iflg |= DONE+FAIL; break;}
      else iAreaOffset++;
      if ((++iMaskOffset >= iMaskLen) && (iAreaOffset >= iAreaLen))
      { iflg |= DONE+MATCH; break;}
    }
  }
  if (iflg & DONE) return;

    /* Look for mask string terminator. */
    /* if no  terminator:  mask ends:    '*' marks suffix */
    /* if '*' terminator:  mask float:   process floating mask */

  ixMaskOffset=iMaskOffset;
  while (++ixMaskOffset < iMaskLen)
  { if (*cA == *(cMask+ixMaskOffset)) break;}
  if (ixMaskOffset >= iMaskLen)         /* Mask is suffix */
  { iyMaskOffset = ixMaskOffset-1;      /* ptr last char in mask */
    iyAreaOffset = iAreaLen - 1;        /* ptr last char in area */
    while (TRUE)
    { if (iflg & DONE) break;
      if (iyMaskOffset < iMaskOffset) iflg |= DONE+MATCH;
      else
      { if (iyAreaOffset < iAreaOffset) iflg |= DONE+FAIL;
        else
        { if (*(cMask+iyMaskOffset) != *(cArea+iyAreaOffset))
          { if (*(cMask+iyMaskOffset) == *cQ)
            {iyMaskOffset--; iyAreaOffset--;}
            else iflg |= DONE+FAIL;
          }
        }
      }
      iyMaskOffset--; iyAreaOffset--;
    }
    goto AStringReturn;                 /* return from AString */
  }

  /* We have case of imbedded *xxx* mask string */

  iwMaskLen=ixMaskOffset-iMaskOffset;   /* length of *xxx* string or zero */
  ixAreaOffset=iAreaOffset;             /* save ptr area */
  iMatchCount=0;                        /* count of matched chars */
  while (ixAreaOffset < iAreaLen)       /* stop at end of area if not sooner */
  { if (iwMaskLen <= 0) break;          /* no-op loop if length is zero */
    if (iMatchCount > 0) break;         /* stop loop if here and not zero */
    iyAreaOffset = ixAreaOffset;
    iyMaskOffset = iMaskOffset;
    while (iyMaskOffset < ixMaskOffset)
    { if (iMatchCount >= iwMaskLen) break; /* stop if matches >= mask length */
      if (*(cMask+iyMaskOffset) == *(cArea+iyAreaOffset))
      { iyMaskOffset++; iyAreaOffset++; iMatchCount++; }
      else
      { if (*(cMask+iyMaskOffset) == *cQ)
        { iMatchCount++; iyAreaOffset++; iyMaskOffset++; }
        else { iMatchCount = 0; break; }
      }
    }
    if (iMatchCount < iwMaskLen) iMatchCount = 0;
    ixAreaOffset++;
  }
  if ((iMatchCount >= iwMaskLen) && (iwMaskLen > 0))
  { iflg |= FLTFND;
    iMaskOffset = ixMaskOffset; iAreaOffset = iyAreaOffset;
    if (iMaskOffset >= iMaskLen) iflg |= DONE+MATCH;
  }
AStringReturn:
  return;
}
/*=============
  PrefixString
=============*/
  static void PrefixString (void)
{ unsigned long ipMaskOffset;
  ipMaskOffset = iMaskOffset;
  while (++ipMaskOffset < iMaskLen)
  { if (*cA == *(cMask+ipMaskOffset)) break;}
  if (ipMaskOffset > iAreaLen) { iflg |= DONE+FAIL; return;}
  while (TRUE)
  { if (iflg & DONE) break;
    if (*cA == *(cMask+iMaskOffset)) break;
    if (iAreaOffset >= iAreaLen)
    { if (iMaskOffset >= iMaskLen) iflg |= DONE+MATCH; break;}
    if (iMaskOffset >= iMaskLen)
    { if (iAreaOffset >= iAreaLen) iflg |= DONE+MATCH;
      else iflg |= DONE+FAIL; break;
    }
    if (*(cMask+iMaskOffset) != *(cArea+iAreaOffset))
    { if (*cA != *(cMask+iMaskOffset))
      { iflg |= DONE+FAIL; break;}
    }
    iMaskOffset++; iAreaOffset++;
  }
  return;
}


/*=============
  mskchk
=============*/

  unsigned long mskchk ( char *msk,     /* ptr mask string   */
                unsigned long msklen,   /* length of mask    */
               char *achk,              /* ptr area string   */
               unsigned long alen,      /* length of area    */
               char *q,                 /* ptr qmark char    */
               char *a )                /* ptr asterisk char */

{ cMask=msk; iMaskLen=msklen; cArea=achk; iAreaLen=alen; cQ=q; cA=a;
  irc=0; iflg=0; iMaskOffset=0; iAreaOffset = 0;
  if (msk==NULL||achk==NULL||q==NULL||a==NULL||msklen<=0||alen<=0)
  { return ERROR;}                      /* Error if null argument(s) */

  while (*(cArea+iAreaOffset) == ' ')   /* Scan for 1st non-blank */
  { iAreaOffset++; if (iAreaOffset >= iAreaLen) return WARNING;}
    /* Scan for last non-blank and non-null */
  while (*(cArea+iAreaLen-1) == ' ' || *(cArea+iAreaLen-1) == '\0')
  { iAreaLen--; if (iAreaLen <= iAreaOffset) return ERROR;}

  while (!(iflg & DONE))
  { if (iMaskOffset >= iMaskLen) break;
    while (*cQ == *(cMask+iMaskOffset)) /* while '?' in mask */
    { if (iflg & DONE) break;
      iMaskOffset++; iAreaOffset++;
      if (iAreaOffset >= iAreaLen)          /* if area is exhausted */
      { if (iMaskOffset >= iMaskLen)        /* and mask is exhausted */
        { iflg |= DONE+MATCH;}               /* we're done and matched */
        else
        { if ((iMaskOffset+1>=iMaskLen) && (*cA == *(cMask+iMaskOffset+1)))
          { iflg |= DONE+MATCH;}            /* if the last mask char is ast */
          else                              /* we're matched */
          { iflg |= DONE+FAIL;}             /* otherwise failed */
        } break;
      }
      if (iMaskOffset >= iMaskLen)
      { iflg |= DONE;
        if (iAreaOffset < iAreaLen)
        { if (*cQ == *(cMask+iMaskOffset-1)) iflg |= FAIL; }
        break;
      }
    }
    if (iflg & DONE) break;
    if (*cA == *(cMask+iMaskOffset))    /* start of float str */
    { iMaskOffset++;
      if (iMaskOffset >= iMaskLen)          /* if mask is exhausted */
      { iflg |= DONE;
        if (iAreaOffset < iAreaLen)         /* but area is not      */
        { if (*cA==*(cMask+iMaskOffset-1))  /* but last mask char is asterisk */
          { iflg |= MATCH;}
        }
        else  iflg |= MATCH;           /* area is complete */
        break;
      }
      AString();
    }
    else PrefixString();
  }
  if (iflg & FAIL) irc = 8;
  else irc = 0;
  /* printf("MSKCHKRC=%i\n",irc); */
  return irc;
}
