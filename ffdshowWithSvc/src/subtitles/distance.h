#ifndef DISTANCE_H
#define DISTANCE_H

class EditDistance {
private:
    ints d;
    /*Gets the minimum of three values */
    static inline int minimum( const int a, const int b, const int c )
    {
        int min = a;
        if ( b < min )
              min = b;
        if ( c < min )
              min = c;
          return min;
    };
public:
    EditDistance(  );
    int CalEditDistance( const char_t *s, const char_t *t, int limit );
};

#endif
