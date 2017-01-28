#include "njanson_DS3231.h"

njanson_DS3231::njanson_DS3231() {
}

bool njanson_DS3231::begin() {
	Wire.begin();

	// Test that we initalized correctly.
	return Read();
}

uint8_t njanson_DS3231::BCDToDecimal(uint8_t bcd) {
	return ((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F);
}

uint8_t njanson_DS3231::DecimalToBCD(uint8_t dec) {
	return ((dec / 10) << 4) | (dec % 10);
}

uint8_t njanson_DS3231::second() {
	return _second;
}

uint8_t njanson_DS3231::minute() {
	return _minute;
}

uint8_t njanson_DS3231::hour() {
	return _hour;
}

uint8_t njanson_DS3231::day() {
	return _day;
}

uint8_t njanson_DS3231::weekDay() {
	return _weekDay;
}

uint8_t njanson_DS3231::month() {
	return _month;
}

uint16_t njanson_DS3231::year() {
	return _year;
}

bool njanson_DS3231::Read() {
	Wire.beginTransmission(DS3231_RTC_ADDR);
	Wire.write(DS3231_SECONDS_REG);  // Start reading at the beginning.
	Wire.endTransmission();

	// Registers 0-7 are the date registers.  See njanson_DS3231.h for more
	// details on specific register layouts and how the fields are
	// structured.
	Wire.requestFrom(DS3231_RTC_ADDR, 7);
	uint8_t bytes[7] = {0, 0, 0, 0, 0, 0, 0};
	for (int i = 0; i < 7; i++) {
		bytes[i] = Wire.read();
	}

	_second = BCDToDecimal(bytes[0]);
	_minute = BCDToDecimal(bytes[1]);

	// Disregard 12/24 bits.  Assume 24h time.
	_hour = BCDToDecimal(bytes[2] & 0b00111111);

	_weekDay = BCDToDecimal(bytes[3]);
	_day = BCDToDecimal(bytes[4]);
	_month = BCDToDecimal(bytes[5] & 0b00011111);
	_year = BCDToDecimal(bytes[6]) + DS3231_CENTURY;
	return true;
}

bool njanson_DS3231::SetClock(uint16_t year,
		              uint8_t month,
			      uint8_t day,
			      uint8_t weekDay,
			      uint8_t hour,
			      uint8_t minute,
			      uint8_t second) {
	uint8_t bytes[7] = {0, 0, 0, 0, 0, 0, 0};

	// Though arguments to this function proceed from most significant to
	// least significant value, that's the opposite of how the DS3231
	// stores its data.  We write from seconds on up to take advantage of
	// the Wire library's ability to send multiple bytes of data at once.
	bytes[0] = DecimalToBCD(second);
	bytes[1] = DecimalToBCD(minute);
	bytes[2] = DecimalToBCD(hour) | DS3231_24H_BIT;
	bytes[3] = DecimalToBCD(weekDay);
	bytes[4] = DecimalToBCD(day);
	bytes[5] = DecimalToBCD(month);
	bytes[6] = DecimalToBCD(year - DS3231_CENTURY);

	Wire.beginTransmission(DS3231_RTC_ADDR);
	// Begin writing at the seconds registry.
	Wire.write(DS3231_SECONDS_REG);
	int written = Wire.write(bytes, sizeof(bytes));
	Wire.endTransmission();
	if (written != sizeof(bytes)) {
		return false;
	}
	Serial.print("Wrote bytes: ");
	Serial.println(written);

	// Now that the data are written, overwrite our internal state.
	_second = second;
	_minute = minute;
	_hour = hour;
	_weekDay = weekDay;
	_day = day;
	_month = month;
	_year = year;
	return true;
}
