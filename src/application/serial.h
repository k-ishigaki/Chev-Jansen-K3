/**
 * Initialize serial module.
 *
 * To use printf function, {@code init_serial()} must be called first.
 */
void init_serial();

int printf(const char *fmt, ...);
/**
 * 引数で指定された文字列を、'\0'が来るまで連続で出力し続けます．
 * @param str 文字列ポインタ
 */
void printstr(char *str);
/**
 * 引数で指定された数値を、引数で指定された基数に従って文字列に変換し、表示します．
 * @param num 表示する文字列
 * @param radix 基数
 */
void printint(int16_t num, uint16_t radix);
/**
 * 引数で指定された文字列を、'\0'が来るまで連続で出力し続けます．
 * 最後に改行を出力します．
 * @param str 文字列ポインタ
 */
void printlnstr(char *str);
/**
 * 引数で指定された数値を、引数で指定された基数に従って文字列に変換し、表示します．
 * 最後に改行を出力します．
 * @param num 表示する文字列
 * @param radix 基数
 */
void printlnint(int16_t num, uint16_t radix);
/**
 * バッファから一文字読み出します．
 * この時、エコーバックはしません．
 * また、バッファが空の時は-1を返します．
 * @return バッファの先頭の文字．バッファが空の時は-1．
 */
char getch();
/**
 * 引数で指定された文字を出力します
 * @param c 出力する文字．
 */
void putch(char c);
