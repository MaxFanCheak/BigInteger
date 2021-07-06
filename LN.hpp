#ifndef LN_LN_H
#define LN_LN_H

#include <string_view>

using namespace std;

class LN {
public:

	explicit LN(long long number = 0);

	LN(LN const &copy);

	LN(LN &&copy) noexcept;

	explicit LN(bool number);

	explicit LN(string_view number);

	explicit LN(const char *number);

	~LN();

	LN &operator=(LN &&copy) noexcept;

	LN &operator=(const LN &copy);

	LN operator+(const LN &number) const;

	LN operator-(const LN &number) const;

	LN operator*(const LN &number) const;

	LN operator/(const LN &number) const;

	LN operator%(const LN &number) const;

	LN &operator+=(const LN &number);

	LN &operator-=(const LN &number);

	LN &operator*=(const LN &number);

	LN &operator/=(const LN &number);

	LN &operator%=(const LN &number);

	LN operator~() const;

	LN operator-() const;

	bool operator>(const LN &number) const;

	bool operator==(const LN &number) const;

	bool operator<(const LN &number) const;

	bool operator>=(const LN &number) const;

	bool operator<=(const LN &number) const;

	bool operator!=(const LN &number) const;

	explicit operator std::string() const;

	explicit operator long long() const;

	explicit operator bool() const;

private:
	int *digits_ = nullptr;
	unsigned int cntDigits_;
	bool NaN_;
	bool notPositive_;

	[[nodiscard]] int compare(const LN &number) const;

	[[nodiscard]] int abstractCompare(const LN &number) const;

	int *add(const LN &number);
	int *sub(const LN &number);
	void removeZeros();
};


#endif //LN_LN_H
