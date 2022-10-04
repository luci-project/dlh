#include <dlh/stream/output.hpp>
#include <dlh/syscall.hpp>
#include <dlh/string.hpp>
#include <dlh/stdarg.hpp>

void check_helper(const char * output, const char * argstr, const char * format, ...) {
	StringStream<512> tmp{};
	cout << "printf(\"" << format << '"';
	if (String::len(argstr) > 0)
		cout << ", " << argstr;
	va_list args;
	va_start(args, format);
	size_t ret = tmp.format(format, args);
	cout << ") = " << ret << endl << " -> \"" << tmp << '"' << endl;
	va_end(args);
	if (ret != String::len(output)) {
		cerr << "Invalid return value, " << String::len(output) << " expected!" << endl;
		Syscall::exit(1);
	}
	if (String::compare(output, tmp.str()) != 0) {
		cerr << "Invalid output, expecting:" << endl << " -> \"" << output << '"' << endl;
		Syscall::exit(1);
	}
	cout << endl;
}


#define check(OUTPUT, FORMAT, ...) check_helper(OUTPUT, #__VA_ARGS__, FORMAT __VA_OPT__(,) __VA_ARGS__)

int main(int argc, const char *argv[]) {
	(void) argc;
	(void) argv;

	// Taken from https://github.com/BartMassey/printf-tests/blob/master/printf-tests.txt

	check("", "%c", 0x7f);

	check("+hello+", "+%s+", "hello");
	check("+10+", "+%d+", 10);
	check("a", "%c", 'a');
	check("\"", "%c", 34);
	check("$", "%c", 36);
	check("10", "%d", 10);

	check("%10x", "%%10x", 42);

	check("foobar", "%3s", "foobar");
	check("                          foobar", "%32s", "foobar");
	check("", "%.s", "foobar");
	check("foo", "%.3s", "foobar");
	check("foo", "%.*s", 3, "foobar");

	check(" 42", "% d", 42);
	check("-42", "% d", -42);
	check("   42", "% 5d", 42);
	check("  -42", "% 5d", -42);
	check("             42", "% 15d", 42);
	check("            -42", "% 15d", -42);

	check("+42", "%+d", 42);
	check("-42", "%+d", -42);
	check("  +42", "%+5d", 42);
	check("  -42", "%+5d", -42);
	check("            +42", "%+15d", 42);
	check("            -42", "%+15d", -42);

	check("42", "%0d", 42);
	check("-42", "%0d", -42);
	check("00042", "%05d", 42);
	check("-0042", "%05d", -42);
	check("000000000000042", "%015d", 42);
	check("-00000000000042", "%015d", -42);

	check("42", "%-d", 42);
	check("-42", "%-d", -42);
	check("42   ", "%-5d", 42);
	check("-42  ", "%-5d", -42);
	check("42             ", "%-15d", 42);
	check("-42            ", "%-15d", -42);
	check("42", "%-0d", 42);
	check("-42", "%-0d", -42);
	check("42   ", "%-05d", 42);
	check("-42  ", "%-05d", -42);
	check("42             ", "%-015d", 42);
	check("-42            ", "%-015d", -42);
	check("42", "%0-d", 42);
	check("-42", "%0-d", -42);
	check("42   ", "%0-5d", 42);
	check("-42  ", "%0-5d", -42);
	check("42             ", "%0-15d", 42);
	check("-42            ", "%0-15d", -42);

	check(" foo", "%*s", 4, "foo");

	check("+hello+", "+%s+", "hello");
	check("+10+", "+%d+", 10);
	check("a", "%c", 'a');
	check(" ", "%c", 32);
	check("$", "%c", 36);
	check("10", "%d", 10);
	check("0", "%d", 0);

	check("    +100", "%+8lld", 100LL);
	check("+00000100", "%+.8lld", 100LL);
	check(" +00000100", "%+10.8lld", 100LL);

	check("-00100", "%-1.5lld", -100LL);
	check("  100", "%5lld", 100LL);
	check(" -100", "%5lld", -100LL);
	check("100  ", "%-5lld", 100LL);
	check("-100 ", "%-5lld", -100LL);
	check("00100", "%-.5lld", 100LL);
	check("-00100", "%-.5lld", -100LL);
	check("00100   ", "%-8.5lld", 100LL);
	check("-00100  ", "%-8.5lld", -100LL);
	check("00100", "%05lld", 100LL);
	check("-0100", "%05lld", -100LL);
	check(" 100", "% lld", 100LL);
	check("-100", "% lld", -100LL);
	check("  100", "% 5lld", 100LL);
	check(" -100", "% 5lld", -100LL);
	check(" 00100", "% .5lld", 100LL);
	check("-00100", "% .5lld", -100LL);
	check("   00100", "% 8.5lld", 100LL);
	check("  -00100", "% 8.5lld", -100LL);
	check("", "%.0lld", 0LL);
	check(" 0x00ffffffffffffff9c", "%#+21.18llx", -100LL);
	check("0001777777777777777777634", "%#.25llo", -100LL);
	check(" 01777777777777777777634", "%#+24.20llo", -100LL);
	check("0X00000FFFFFFFFFFFFFF9C", "%#+18.21llX", -100LL);
	check("001777777777777777777634", "%#+20.24llo", -100LL);
	check("   0018446744073709551615", "%#+25.22llu", -1LL);
	check("   0018446744073709551615", "%#+25.22llu", -1LL);
	check("     0000018446744073709551615", "%#+30.25llu", -1LL);
	check("  -0000000000000000000001", "%+#25.22lld", -1LL);
	check("00144   ", "%#-8.5llo", 100LL);
	check("+00100  ", "%#-+ 08.5lld", 100LL);
	check("+00100  ", "%#-+ 08.5lld", 100LL);
	check("0000000000000000000000000000000000000001", "%.40lld", 1LL);
	check(" 0000000000000000000000000000000000000001", "% .40lld", 1LL);
	check(" 0000000000000000000000000000000000000001", "% .40d", 1);

 	check("%I", "%I", 1);
	check("1", "%I0d", 1);
	check("                               1", "%I32d", 1);
 	check("%llD", "%llD", -1LL);

	check(" 1", "% d", 1);
	check("+1", "%+ d", 1);

	check("0001", "%04c", '1');
	check("1   ", "%-04c", '1');
	check("0x0000000001", "%#012x", 1);
	check("0x00000001", "%#04.8x", 1);
	check("0x01    ", "%#-08.2x", 1);
	check("00000001", "%#08o", 1);

	check("(null)", "%s", nullptr);
	check("    (null)", "%10s", nullptr);

	check("(nil)", "%p", nullptr);
	check("     (nil)", "%010p", nullptr);
 	check("0x39", "%p", (void*)57);

	check("0x000000000000000039", "%#020p", (void*)57);
	check("0x39                ", "%#-020p", (void*)57);

	check("0x0000000039", "%#012p", (void*)57);
	check("0x39        ", "%#-012p", (void*)57);
	check("0foo", "%04s", "foo");

	check("f", "%.1s", "foo");
	check("f", "%.*s", 1, "foo");
	check("foo  ", "%*s", -5, "foo");
	check("hello", "hello");

	check("  a", "%3c", 'a');
	check("1234", "%3d", 1234);

	check("2", "%-1d", 2);

	check("-1", "%-i", -1);
	check("1", "%-i", 1);
	check("+1", "%+i", 1);
	check("12", "%o", 10);

	check("(null)", "%s", 0);
	check("%%%%", "%s", "%%%%");
	check("4294967295", "%u", -1);

	check("%0", "%%0", );

	check("Hallo heimur", "Hallo heimur");
	check("Hallo heimur", "%s", "Hallo heimur");
	check("1024", "%d", 1024);
	check("-1024", "%d", -1024);
	check("1024", "%i", 1024);
	check("-1024", "%i", -1024);
	check("1024", "%u", 1024);
	check("4294966272", "%u", 4294966272U);
	check("777", "%o", 511);
	check("37777777001", "%o", 4294966785U);
	check("1234abcd", "%x", 305441741);
	check("edcb5433", "%x", 3989525555U);
	check("1234ABCD", "%X", 305441741);
	check("EDCB5433", "%X", 3989525555U);
	check("x", "%c", 'x');
	check("%", "%%");

	check("Hallo heimur", "%+s", "Hallo heimur");
	check("+1024", "%+d", 1024);
	check("-1024", "%+d", -1024);
	check("+1024", "%+i", 1024);
	check("-1024", "%+i", -1024);
	check("1024", "%+u", 1024);
	check("4294966272", "%+u", 4294966272U);
	check("777", "%+o", 511);
	check("37777777001", "%+o", 4294966785U);
	check("1234abcd", "%+x", 305441741);
	check("edcb5433", "%+x", 3989525555U);
	check("1234ABCD", "%+X", 305441741);
	check("EDCB5433", "%+X", 3989525555U);
	check("x", "%+c", 'x');

	check("Hallo heimur", "% s", "Hallo heimur");
	check(" 1024", "% d", 1024);
	check("-1024", "% d", -1024);
	check(" 1024", "% i", 1024);
	check("-1024", "% i", -1024);
	check("1024", "% u", 1024);
	check("4294966272", "% u", 4294966272U);
	check("777", "% o", 511);
	check("37777777001", "% o", 4294966785U);
	check("1234abcd", "% x", 305441741);
	check("edcb5433", "% x", 3989525555U);
	check("1234ABCD", "% X", 305441741);
	check("EDCB5433", "% X", 3989525555U);
	check("x", "% c", 'x');

	check("Hallo heimur", "%+ s", "Hallo heimur");
	check("+1024", "%+ d", 1024);
	check("-1024", "%+ d", -1024);
	check("+1024", "%+ i", 1024);
	check("-1024", "%+ i", -1024);
	check("1024", "%+ u", 1024);
	check("4294966272", "%+ u", 4294966272U);
	check("777", "%+ o", 511);
	check("37777777001", "%+ o", 4294966785U);
	check("1234abcd", "%+ x", 305441741);
	check("edcb5433", "%+ x", 3989525555U);
	check("1234ABCD", "%+ X", 305441741);
	check("EDCB5433", "%+ X", 3989525555U);
	check("x", "%+ c", 'x');

	check("0777", "%#o", 511);
	check("037777777001", "%#o", 4294966785U);
	check("0x1234abcd", "%#x", 305441741);
	check("0xedcb5433", "%#x", 3989525555U);
	check("0X1234ABCD", "%#X", 305441741);
	check("0XEDCB5433", "%#X", 3989525555U);
	check("0", "%#o", 0U);
	check("0x0", "%#x", 0U);
	check("0X0", "%#X", 0U);

	check("Hallo heimur", "%1s", "Hallo heimur");
	check("1024", "%1d", 1024);
	check("-1024", "%1d", -1024);
	check("1024", "%1i", 1024);
	check("-1024", "%1i", -1024);
	check("1024", "%1u", 1024);
	check("4294966272", "%1u", 4294966272U);
	check("777", "%1o", 511);
	check("37777777001", "%1o", 4294966785U);
	check("1234abcd", "%1x", 305441741);
	check("edcb5433", "%1x", 3989525555U);
	check("1234ABCD", "%1X", 305441741);
	check("EDCB5433", "%1X", 3989525555U);
	check("x", "%1c", 'x');

	check("               Hallo", "%20s", "Hallo");
	check("                1024", "%20d", 1024);
	check("               -1024", "%20d", -1024);
	check("                1024", "%20i", 1024);
	check("               -1024", "%20i", -1024);
	check("                1024", "%20u", 1024);
	check("          4294966272", "%20u", 4294966272U);
	check("                 777", "%20o", 511);
	check("         37777777001", "%20o", 4294966785U);
	check("            1234abcd", "%20x", 305441741);
	check("            edcb5433", "%20x", 3989525555U);
	check("            1234ABCD", "%20X", 305441741);
	check("            EDCB5433", "%20X", 3989525555U);
	check("                   x", "%20c", 'x');

	check("Hallo               ", "%-20s", "Hallo");
	check("1024                ", "%-20d", 1024);
	check("-1024               ", "%-20d", -1024);
	check("1024                ", "%-20i", 1024);
	check("-1024               ", "%-20i", -1024);
	check("1024                ", "%-20u", 1024);
	check("4294966272          ", "%-20u", 4294966272U);
	check("777                 ", "%-20o", 511);
	check("37777777001         ", "%-20o", 4294966785U);
	check("1234abcd            ", "%-20x", 305441741);
	check("edcb5433            ", "%-20x", 3989525555U);
	check("1234ABCD            ", "%-20X", 305441741);
	check("EDCB5433            ", "%-20X", 3989525555U);
	check("x                   ", "%-20c", 'x');

	check("00000000000000001024", "%020d", 1024);
	check("-0000000000000001024", "%020d", -1024);
	check("00000000000000001024", "%020i", 1024);
	check("-0000000000000001024", "%020i", -1024);
	check("00000000000000001024", "%020u", 1024);
	check("00000000004294966272", "%020u", 4294966272U);
	check("00000000000000000777", "%020o", 511);
	check("00000000037777777001", "%020o", 4294966785U);
	check("0000000000001234abcd", "%020x", 305441741);
	check("000000000000edcb5433", "%020x", 3989525555U);
	check("0000000000001234ABCD", "%020X", 305441741);
	check("000000000000EDCB5433", "%020X", 3989525555U);

	check("                0777", "%#20o", 511);
	check("        037777777001", "%#20o", 4294966785U);
	check("          0x1234abcd", "%#20x", 305441741);
	check("          0xedcb5433", "%#20x", 3989525555U);
	check("          0X1234ABCD", "%#20X", 305441741);
	check("          0XEDCB5433", "%#20X", 3989525555U);

	check("00000000000000000777", "%#020o", 511);
	check("00000000037777777001", "%#020o", 4294966785U);
	check("0x00000000001234abcd", "%#020x", 305441741);
	check("0x0000000000edcb5433", "%#020x", 3989525555U);
	check("0X00000000001234ABCD", "%#020X", 305441741);
	check("0X0000000000EDCB5433", "%#020X", 3989525555U);

	check("Hallo               ", "%0-20s", "Hallo");
	check("1024                ", "%0-20d", 1024);
	check("-1024               ", "%0-20d", -1024);
	check("1024                ", "%0-20i", 1024);
	check("-1024               ", "%0-20i", -1024);
	check("1024                ", "%0-20u", 1024);
	check("4294966272          ", "%0-20u", 4294966272U);
	check("777                 ", "%-020o", 511);
	check("37777777001         ", "%-020o", 4294966785U);
	check("1234abcd            ", "%-020x", 305441741);
	check("edcb5433            ", "%-020x", 3989525555U);
	check("1234ABCD            ", "%-020X", 305441741);
	check("EDCB5433            ", "%-020X", 3989525555U);
	check("x                   ", "%-020c", 'x');

	check("               Hallo", "%*s", 20, "Hallo");
	check("                1024", "%*d", 20, 1024);
	check("               -1024", "%*d", 20, -1024);
	check("                1024", "%*i", 20, 1024);
	check("               -1024", "%*i", 20, -1024);
	check("                1024", "%*u", 20, 1024);
	check("          4294966272", "%*u", 20, 4294966272U);
	check("                 777", "%*o", 20, 511);
	check("         37777777001", "%*o", 20, 4294966785U);
	check("            1234abcd", "%*x", 20, 305441741);
	check("            edcb5433", "%*x", 20, 3989525555U);
	check("            1234ABCD", "%*X", 20, 305441741);
	check("            EDCB5433", "%*X", 20, 3989525555U);
	check("                   x", "%*c", 20, 'x');

	check("Hallo heimur", "%.20s", "Hallo heimur");
	check("00000000000000001024", "%.20d", 1024);
	check("-00000000000000001024", "%.20d", -1024);
	check("00000000000000001024", "%.20i", 1024);
	check("-00000000000000001024", "%.20i", -1024);
	check("00000000000000001024", "%.20u", 1024);
	check("00000000004294966272", "%.20u", 4294966272U);
	check("00000000000000000777", "%.20o", 511);
	check("00000000037777777001", "%.20o", 4294966785U);
	check("0000000000001234abcd", "%.20x", 305441741);
	check("000000000000edcb5433", "%.20x", 3989525555U);
	check("0000000000001234ABCD", "%.20X", 305441741);
	check("000000000000EDCB5433", "%.20X", 3989525555U);

	check("               Hallo", "%20.5s", "Hallo heimur");
	check("               01024", "%20.5d", 1024);
	check("              -01024", "%20.5d", -1024);
	check("               01024", "%20.5i", 1024);
	check("              -01024", "%20.5i", -1024);
	check("               01024", "%20.5u", 1024);
	check("          4294966272", "%20.5u", 4294966272U);
	check("               00777", "%20.5o", 511);
	check("         37777777001", "%20.5o", 4294966785U);
	check("            1234abcd", "%20.5x", 305441741);
	check("          00edcb5433", "%20.10x", 3989525555U);
	check("            1234ABCD", "%20.5X", 305441741);
	check("          00EDCB5433", "%20.10X", 3989525555U);

//This test is undefined. Common sense says libc fails it.
	check("               Hallo", "%020.5s", "Hallo heimur");
	check("               01024", "%020.5d", 1024);
	check("              -01024", "%020.5d", -1024);
	check("               01024", "%020.5i", 1024);
	check("              -01024", "%020.5i", -1024);
	check("               01024", "%020.5u", 1024);
	check("          4294966272", "%020.5u", 4294966272U);
	check("               00777", "%020.5o", 511);
	check("         37777777001", "%020.5o", 4294966785U);
	check("            1234abcd", "%020.5x", 305441741);
	check("          00edcb5433", "%020.10x", 3989525555U);
	check("            1234ABCD", "%020.5X", 305441741);
	check("          00EDCB5433", "%020.10X", 3989525555U);

	check("", "%.0s", "Hallo heimur");
	check("                    ", "%20.0s", "Hallo heimur");
	check("", "%.s", "Hallo heimur");
	check("                    ", "%20.s", "Hallo heimur");
	check("                1024", "%20.0d", 1024);
	check("               -1024", "%20.d", -1024);
	check("                    ", "%20.d", 0);
	check("                1024", "%20.0i", 1024);
	check("               -1024", "%20.i", -1024);
	check("                    ", "%20.i", 0);
	check("                1024", "%20.u", 1024);
	check("          4294966272", "%20.0u", 4294966272U);
	check("                    ", "%20.u", 0U);
	check("                 777", "%20.o", 511);
	check("         37777777001", "%20.0o", 4294966785U);
	check("                    ", "%20.o", 0U);
	check("            1234abcd", "%20.x", 305441741);
	check("            edcb5433", "%20.0x", 3989525555U);
	check("                    ", "%20.x", 0U);
	check("            1234ABCD", "%20.X", 305441741);
	check("            EDCB5433", "%20.0X", 3989525555U);
	check("                    ", "%20.X", 0U);

	check("Hallo               ", "% -0+20.5s", "Hallo heimur");
	check("Hallo               ", "% -0+*.*s", 20, 5, "Hallo heimur");
	check("+01024              ", "% -0+*.*d", 20, 5, 1024);
	check("-01024              ", "% -0+*.*d", 20, 5, -1024);
	check("+01024              ", "% -0+*.*i", 20, 5, 1024);
	check("-01024              ", "% 0-+*.*i", 20, 5, -1024);
	check("01024               ", "% 0-+*.*u", 20, 5, 1024);
	check("4294966272          ", "% 0-+*.*u", 20, 5, 4294966272U);
	check("00777               ", "%+ -0*.*o", 20, 5, 511);
	check("37777777001         ", "%+ -0*.*o", 20, 5, 4294966785U);
	check("1234abcd            ", "%+ -0*.*x", 20, 5, 305441741);
	check("00edcb5433          ", "%+ -0*.*x", 20, 10, 3989525555U);
	check("1234ABCD            ", "% -+0*.*X", 20, 5, 305441741);
	check("00EDCB5433          ", "% -+0*.*X", 20, 10, 3989525555U);

	check("hi x", "%*sx", -3, "hi");
	return 0;
}
