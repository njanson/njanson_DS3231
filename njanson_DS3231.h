/**
 * Library for DS3231.
 *
 * Datasheet at https://datasheets.maximintegrated.com/en/ds/DS3231.pdf.
 */

#if (ARDUINO >= 100)
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

#include <Wire.h>

#define DS3231_RTC_ADDR 0x68
#define DS3231_ROM_ADDR 0x57

#define DS3231_SECONDS_REG 0x00
#define DS3231_MINUTES_REG 0x01
#define DS3231_HOURS_REG 0x02
#define DS3231_WEEKDAY_REG 0x03
#define DS3231_MONTH_REG 0x04
#define DS3231_YEAR_REG 0x05

// Assume that this software will have a constant century; this will be added
// to the 'year' field when read from the RTC.
#define DS3231_CENTURY 2000

// This field is ORed with the Hour field when writing in order to ensure that
// we always have the 24h clock bit set in the internal representation of time.
#define DS3231_24H_BIT 0b01000000

/**
 * The DS3231 is an accurate real-time clock with second-level precision.  It
 * has an accuracy of +/- 2 minutes per year.
 *
 * The DS3231 has no concept of local time, so this library maintains time in
 * whatever timezone it's told.
 *
 * This library stores time in the 24h format and does not make use of the
 * microcontroller's ability to switch between 24h time and AM/PM.  We ignore
 * the Century bit, which is only toggled on the overflow between 00 and 99;
 * this given that this is 2017 it is highly unlikely that this code will still
 * be running in the next century.
 *
 * Register layout is (per datasheet, with alarms elided as this library does
 * not support them):
 *
 * +------+----------+-----+-----+----+----+----+----+----+----------+---------+
 * | ADDR | BIT7/MSB |BIT6 |BIT5 |BIT4|BIT3|BIT2|BIT1|BIT0| Function |  Range  |
 * | 0x00 | 0        |  10 seconds    |      Seconds      | Seconds  | 00 - 59 |
 * | 0x01 | 0        |  10 minutes    |      Minutes      | Minutes  | 00 - 59 |
 * | 0x02 | 0        |12/24|AM/PM|10H |      Hours        | Hours    | 1-12 OR |
 * |      |          |   **|20H**|    |                   |          | 0-23    |
 * | 0x03 | 0        | 0   | 0   | 0  | 0  | Day          | Day      | 1 - 7   |
 * | 0x04 | 0        | 0   | 10 Date  |      Date         | Date     | 01 - 31 |
 * | 0x05 | Century  | 0   | 0   |10Mo|      Month        | Month/C. | 01-12 +C|
 * | 0x06 | 10 year                   | Year              | Year     | 0-99    |
 * | 0x0E | EOSC     |BBSQW|CONV |RS2 |RS1 |INTCN|A2IE|A1IE|Control  | Varies  |
 * | 0x0F | OSF      | 0   | 0   | 0  |EN32KHz|BSY|A2F|A1F|Ctrl/Status|Varies  |
 * | 0x10 | SIGN     |DATA |DATA |DATA|DATA|DATA|DATA|DATA| Aging Offset       |
 * | 0x11 | SIGN     |DATA |DATA |DATA|DATA|DATA|DATA|DATA| MSB of Temp        |
 * | 0x12 | DATA     |DATA | 0   | 0  | 0  | 0  | 0  | 0  | LSB of Temp        |
 * +------+----------+-----+-----+----+----+----+----+----+--------------------+
 *
 * Note that for address 0x02, when BIT6==1 -> 24H; BIT5==1 -> PM.
 *
 * The data are all stored in binary-coded decimal.
 *
 */
class njanson_DS3231 {
	public:
		njanson_DS3231();
		bool begin();

		uint8_t second();    // Get the most recently read second.
		uint8_t minute();    // Get the most recently read minute.
		uint8_t hour();      // Get the most recently read hour (24h format).
		uint8_t weekDay();   // Get the most recently read day of the week.
		uint8_t day();       // Get the most recently read day.
		uint8_t month();     // Get the most recently read month.
		uint16_t year();     // Get the most recently read year.

		// Conversion functions.  The DS3231 stores its registers in
		// binary-coded decimal, two integers per byte, so we need to
		// be able to convert to and from standard types and BCD.
		uint8_t BCDToDecimal(uint8_t bcd);
		uint8_t DecimalToBCD(uint8_t byte);

		// Update the data structure with the RTC's most up-to-date information.
		// Returns:
		//   True, if we were able to read data.
		//   False otherwise.
		bool Read();

		// Set the clock on the unit.
		// Returns:
		//   True, if we were able to set the clock.
		//   False otherwise.
		bool SetClock(uint16_t year,
		              uint8_t month,
			      uint8_t day,
			      uint8_t weekDay,
			      uint8_t hour,
			      uint8_t minute,
			      uint8_t second);

	private:
		uint8_t _second = 0;
		uint8_t _minute = 0;
		uint8_t _hour = 0;
		uint8_t _weekDay = 0;
		uint8_t _day = 0;
		uint8_t _month = 0;

		// We represent the year with century, even though the DS3231's
		// internal representation is a uint8_t that contains the year
		// in BCD format.  To convert to and from the DS3231's format,
		// we must always remember to add or subtract the century as
		// appropriate.
		uint16_t _year = 0;
};
