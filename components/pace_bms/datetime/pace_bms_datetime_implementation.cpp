
#include "esphome/core/log.h"

#include "pace_bms_datetime_implementation.h"

namespace esphome {
namespace pace_bms_base {

static const char* const TAG = "pace_bms_base.datetime_impl";

void PaceBmsDatetimeImplementation::add_on_control_callback(std::function<void(const datetime::DateTimeCall&)>&& callback) {
	this->control_callbacks_.add(std::move(callback));
}

void PaceBmsDatetimeImplementation::set_datetime(PaceBmsProtocolV25::DateTime& dt) {
	this->year_ = dt.Year;
	this->month_ = dt.Month;
	this->day_ = dt.Day;
	this->hour_ = dt.Hour;
	this->minute_ = dt.Minute;
	this->second_ = dt.Second;
}

void PaceBmsDatetimeImplementation::control(const datetime::DateTimeCall& call) {
	ESP_LOGD(TAG, "control: new value %04hu:%02hu:%02hu %02hu:%02hu:%02hu", call.get_year().value(), call.get_month().value(), call.get_day().value(), call.get_hour().value(), call.get_minute().value(), call.get_second().value());

	// callbacks
	this->control_callbacks_.call(call);

	this->year_ = call.get_year().value();
	this->month_ = call.get_month().value();
	this->day_ = call.get_day().value();
	this->hour_ = call.get_hour().value();
	this->minute_ = call.get_minute().value();
	this->second_ = call.get_second().value();

	// required for the UX not to get out of sync
	this->publish_state();
}

}  // namespace pace_bms_base
}  // namespace esphome
