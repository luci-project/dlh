#include <dlh/datetime.hpp>
#include <dlh/stream/output.hpp>

int main() {
	DateTime m0;
	DateTime m1(2021,2,18,20,55,23,421337000);
	DateTime m2(1509705000);
	DateTime m3(2012,7,37,3,136,116,1042133700);

	cout << m0 << " - " << m0.to_timestamp() << " = " << m0.valid() << endl
	     << m1 << " - " << m1.to_timestamp() << " = " << m1.valid() << endl
	     << m2 << " - " << m2.to_timestamp() << " = " << m2.valid() << endl
	     << m3 << " - " << m3.to_timestamp() << " = " << m3.valid() << endl;

	cout << m1 << (m1 <= m3 ? " was before " : " was after ") << m3 << endl;
	cout << m2 << (m2 >= m0 ? " was after " : " was before ") << m0 << endl;
	cout << m3 << (m3 > m2.to_timestamp() ? " was after " : " was before ") << m2 << endl;
	cout << m0 << (m0 <= m1.to_timespec() ? " was before " : " was after ") << m1 << endl;

	cout << m2.weekday_name() << ", " << m2.month_name() << " " << static_cast<int>(m2.day) << ", " << static_cast<int>(m2.year) << endl;
}
