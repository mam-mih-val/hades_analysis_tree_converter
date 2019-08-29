// Alexander Zaytsev
// a.zaytsev@gsi.de
// October 2016

#ifndef MHWallDivider_h
#define MHWallDivider_h

class MHWallDivider
{
  public:
  MHWallDivider()
  {
  }

  short GetRing(short i)
  {
    if(i < 0 || i > 301)
      return 0;
    // beam hole
    if(i == 65 || i == 66 || i == 77 || i == 78)
      return 0;

    // small modules
    if(i < 144)
    {
      if((i >= 52 && i <= 55) || (i >= 88 && i <= 91) || i == 64 || i == 67 || i == 76 || i == 79)
        return 1;
      if((i >= 39 && i <= 44) || (i >= 99 && i <= 104) || (i >= 51 && i <= 87 && (i - 51) % 12 == 0) ||
          (i >= 56 && i <= 92 && (i - 56) % 12 == 0))
        return 2;
      if((i >= 26 && i <= 33) || (i >= 110 && i <= 117) || (i >= 38 && i <= 98 && (i - 38) % 12 == 0) ||
          (i >= 45 && i <= 105 && (i - 45) % 12 == 0))
        return 3;
      if((i >= 13 && i <= 22) || (i >= 121 && i <= 130) || (i >= 25 && i <= 109 && (i - 25) % 12 == 0) ||
          (i >= 34 && i <= 118 && (i - 34) % 12 == 0))
        return 4;
      else
        return 5;
    }
    // middle modules
    else if(i < 208)
    {
      if((i <= 162 && i >= 155) || (i <= 196 && i >= 189) || (i <= 186 && i >= 165 && (i - 165) % 4 < 2))
        return 6;
      else
        return 7;
    }
    // large modules
    else if(i < 302)
    {
      if((i <= 238 && i >= 232) || (i <= 279 && i >= 273) || (i <= 268 && i >= 243 && (i - 243) % 6 < 2))
        return 8;
      if((i <= 228 && i >= 220) || (i <= 291 && i >= 283) || (i <= 272 && i >= 239 && (i - 242) % 3 == 0) || i == 231 ||
          i == 280)
        return 9;
      else
        return 10;
    }

    return 0;
  }
};
#endif
