#include <dlh/datetime.hpp>
#include <dlh/stream/output.hpp>

int main() {
	DateTime m0;
	DateTime m1(2021,2,18,20,55,23,421337000);
	DateTime m2(1509705000);
	DateTime m3(2012,7,37,3,136,116,1042133700);

	cout << m0 << " - " << m0.toTimestamp() << " = " << m0.valid() << endl
	     << m1 << " - " << m1.toTimestamp() << " = " << m1.valid() << endl
	     << m2 << " - " << m2.toTimestamp() << " = " << m2.valid() << endl
	     << m3 << " - " << m3.toTimestamp() << " = " << m3.valid() << endl;

	cout << m1 << (m1 <= m3 ? " was before " : " was after ") << m3 << endl;
	cout << m2 << (m2 >= m0 ? " was after " : " was before ") << m0 << endl;
	cout << m3 << (m3 > m2.toTimestamp() ? " was after " : " was before ") << m2 << endl;
	cout << m0 << (m0 <= m1.toTimespec() ? " was before " : " was after ") << m1 << endl;
}
