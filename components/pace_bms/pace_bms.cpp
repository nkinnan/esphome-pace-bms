#include "pace_bms.h"
#include "esphome/core/log.h"
#include <cinttypes>
#include <iomanip>
#include <sstream>
#include <functional>

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms";

// for the protocol implementation dependency injection only
static const char* const TAG_V25 = "pace_bms_v25";

/*
* dependency injection to the protocol implementation
*/

void error_log_func(std::string message) {
	ESP_LOGE(TAG_V25, "%s", message.c_str());
}
void warning_log_func(std::string message) {
	ESP_LOGW(TAG_V25, "%s", message.c_str());
}
void info_log_func(std::string message) {
	ESP_LOGI(TAG_V25, "%s", message.c_str());
}
void debug_log_func(std::string message) {
	ESP_LOGD(TAG_V25, "%s", message.c_str());
}
void verbose_log_func(std::string message) {
	ESP_LOGV(TAG_V25, "%s", message.c_str());
}
void very_verbose_log_func(std::string message) {
#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_VERY_VERBOSE
	ESP_LOGVV(TAG_V25, "%s", message.c_str());
#endif
}

/*
* log configuration
*/

void PaceBms::dump_config() {
	ESP_LOGCONFIG(TAG, "pace_bms:");
	LOG_PIN("  Flow Control Pin: ", this->flow_control_pin_);
	ESP_LOGCONFIG(TAG, "  Address: %i", this->address_);
	ESP_LOGCONFIG(TAG, "  Protocol Version: 0x%02X", this->protocol_version_);
	ESP_LOGCONFIG(TAG, "  Request Throttle (ms): %i", this->request_throttle_);
	ESP_LOGCONFIG(TAG, "  Response Timeout (ms): %i", this->response_timeout_);
	this->check_uart_settings(9600);
}

/*
* setup this component
*/

void PaceBms::setup() {
	if (this->protocol_version_ == 0x25) {
		// the protocol en/decoder PaceBmsV25 is meant to be standalone with no dependencies, so inject esphome logging function wrappers on construction
		this->pace_bms_v25_ = new PaceBmsV25(
			(PaceBmsV25::CID1)chemistry_, 
			analog_cell_count_override_, analog_temperature_count_override_,
			design_capacity_mah_override_,
			status_cell_count_override_, status_temperature_count_override_,
			error_log_func, warning_log_func, info_log_func, debug_log_func, verbose_log_func, very_verbose_log_func);
	}
	else if (this->protocol_version_ == 0x20) {
		// the protocol en/decoder PaceBmsV25 is meant to be standalone with no dependencies, so inject esphome logging function wrappers on construction
		this->pace_bms_v20_ = new PaceBmsV20(
			(PaceBmsV20::CID1)chemistry_, 
			v20_skip_address_payload_,
			analog_cell_count_override_, analog_temperature_count_override_,
			v20_skip_ud2_, v20_skip_soc_, v20_skip_dc_, v20_skip_soh_, v20_skip_pv_, 
			design_capacity_mah_override_, 
			status_cell_count_override_, status_temperature_count_override_, 
			error_log_func, warning_log_func, info_log_func, debug_log_func, verbose_log_func, very_verbose_log_func);
	}
	else {
		this->status_set_error();
		ESP_LOGE(TAG, "Protocol version 0x%02X is not supported", this->protocol_version_);
		return;
	}

	if (this->flow_control_pin_ != nullptr)
		this->flow_control_pin_->setup();

	// clear uart buffer
	uint8_t byte;
	while (this->available() != 0) {
		this->read_byte(&byte);
	}
}

/*
* fill read_queue_ with any necessary BMS commands to update sensor values, based on what was subscribed for by child sensor
* instances via setting callbacks to receive the updates
*/

void PaceBms::update() {
	if (this->pace_bms_v25_ == nullptr &&
		this->pace_bms_v20_ == nullptr)
		return;

	// writes are always processed first so no need to check that as well
	if (!read_queue_.empty()) {
		ESP_LOGW(TAG, "Commands still in queue on update(), skipping this refresh cycle: Could not speak with the BMS fast enough: increase update_interval or reduce request_throttle.");
	}
	else {
		if (this->pace_bms_v25_ != nullptr) {
			ESP_LOGV(TAG, "Queueing v25 refresh commands");

			if (this->analog_information_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read analog information");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadAnalogInformationRequest(this->address_, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_analog_information_response_v25(response); };
				read_queue_.push(item);
			}
			if (this->status_information_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read status information");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadStatusInformationRequest(this->address_, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_status_information_response_v25(response); };
				read_queue_.push(item);
			}
			if (this->hardware_version_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read hardware version");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadHardwareVersionRequest(this->address_, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_hardware_version_response_v25(response); };
				read_queue_.push(item);
			}
			if (this->serial_number_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read serial number");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadSerialNumberRequest(this->address_, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_serial_number_response_v25(response); };
				read_queue_.push(item);
			}
			if (this->protocols_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read protocols");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadProtocolsRequest(this->address_, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_protocols_response_v25(response); };
				read_queue_.push(item);
			}
			if (this->cell_over_voltage_configuration_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read cell over voltage configuration");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadConfigurationRequest(this->address_, PaceBmsV25::RC_CellOverVoltage, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_cell_over_voltage_configuration_response_v25(response); };
				read_queue_.push(item);
			}
			if (this->pack_over_voltage_configuration_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read pack over voltage configuration");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadConfigurationRequest(this->address_, PaceBmsV25::RC_PackOverVoltage, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_pack_over_voltage_configuration_response_v25(response); };
				read_queue_.push(item);
			}
			if (this->cell_under_voltage_configuration_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read cell under voltage configuration");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadConfigurationRequest(this->address_, PaceBmsV25::RC_CellUnderVoltage, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_cell_under_voltage_configuration_response_v25(response); };
				read_queue_.push(item);
			}
			if (this->pack_under_voltage_configuration_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read pack under voltage configuration");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadConfigurationRequest(this->address_, PaceBmsV25::RC_PackUnderVoltage, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_pack_under_voltage_configuration_response_v25(response); };
				read_queue_.push(item);
			}
			if (this->charge_over_current_configuration_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read charge over current configuration");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadConfigurationRequest(this->address_, PaceBmsV25::RC_ChargeOverCurrent, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_charge_over_current_configuration_response_v25(response); };
				read_queue_.push(item);
			}
			if (this->discharge_over_current1_configuration_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read discharge over current 1 configuration");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadConfigurationRequest(this->address_, PaceBmsV25::RC_DischargeOverCurrent1, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_discharge_over_current1_configuration_response_v25(response); };
				read_queue_.push(item);
			}
			if (this->discharge_over_current2_configuration_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read discharge over current 2 configuration");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadConfigurationRequest(this->address_, PaceBmsV25::RC_DischargeOverCurrent2, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_discharge_over_current2_configuration_response_v25(response); };
				read_queue_.push(item);
			}
			if (this->short_circuit_protection_configuration_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read short circuit protection configuration");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadConfigurationRequest(this->address_, PaceBmsV25::RC_ShortCircuitProtection, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_short_circuit_protection_configuration_response_v25(response); };
				read_queue_.push(item);
			}
			if (this->cell_balancing_configuration_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read cell balancing configuration");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadConfigurationRequest(this->address_, PaceBmsV25::RC_CellBalancing, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_cell_balancing_configuration_response_v25(response); };
				read_queue_.push(item);
			}
			if (this->sleep_configuration_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read sleep configuration");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadConfigurationRequest(this->address_, PaceBmsV25::RC_Sleep, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_sleep_configuration_response_v25(response); };
				read_queue_.push(item);
			}
			if (this->full_charge_low_charge_configuration_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read full charge low charge configuration");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadConfigurationRequest(this->address_, PaceBmsV25::RC_FullChargeLowCharge, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_full_charge_low_charge_configuration_response_v25(response); };
				read_queue_.push(item);
			}
			if (this->charge_and_discharge_over_temperature_configuration_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read charge and discharge over temperature configuration");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadConfigurationRequest(this->address_, PaceBmsV25::RC_ChargeAndDischargeOverTemperature, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_charge_and_discharge_over_temperature_configuration_response_v25(response); };
				read_queue_.push(item);
			}
			if (this->charge_and_discharge_under_temperature_configuration_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read charge and discharge under temperature configuration");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadConfigurationRequest(this->address_, PaceBmsV25::RC_ChargeAndDischargeUnderTemperature, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_charge_and_discharge_under_temperature_configuration_response_v25(response); };
				read_queue_.push(item);
			}
			if (this->system_datetime_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read system date/time");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadSystemDateTimeRequest(this->address_, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_system_datetime_response_v25(response); };
				read_queue_.push(item);
			}
			if (this->mosfet_over_temperature_configuration_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read mosfet over temperature configuration");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadConfigurationRequest(this->address_, PaceBmsV25::RC_MosfetOverTemperature, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_mosfet_over_temperature_configuration_response_v25(response); };
				read_queue_.push(item);
			}
			if (this->environment_over_under_temperature_configuration_callbacks_v25_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read environment over/under temperature configuration");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateReadConfigurationRequest(this->address_, PaceBmsV25::RC_EnvironmentOverUnderTemperature, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_environment_over_under_temperature_configuration_response_v25(response); };
				read_queue_.push(item);
			}
		}
		else if (this->pace_bms_v20_ != nullptr) {
			ESP_LOGV(TAG, "Queueing v20 refresh commands");

			if (this->analog_information_callbacks_v20_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read analog information");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v20_->CreateReadAnalogInformationRequest(this->address_, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_analog_information_response_v20(response); };
				read_queue_.push(item);
			}
			if (this->status_information_callbacks_v20_.size() > 0) {
				command_item* item = new command_item;
				item->description_ = std::string("read status information");
				item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v20_->CreateReadStatusInformationRequest(this->address_, request); };
				item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_read_status_information_response_v20(response); };
				read_queue_.push(item);
			}
		}

		ESP_LOGV(TAG, "Read commands queued: %i", read_queue_.size());
	}
}

/*
* incrementally process incoming bytes off the bus, eventually dispatching a full response to process_response_frame_
* once request_throttle has been satisfied and no request is outstanding, call send_next_request_frame to continue popping the read/write queues
*/

void PaceBms::loop() {
	if (this->pace_bms_v25_ == nullptr &&
		this->pace_bms_v20_ == nullptr)
		return;

	// if there is no request active, throw away any incoming data before proceeding
	if (this->request_outstanding_ == false &&
		this->available() != 0) {
		ESP_LOGV(TAG, "Throwing away incoming data because there is no request active");
		uint8_t byte;
		while (this->available() != 0) {
			this->read_byte(&byte);
		}
	}

	const uint32_t now = millis();

	// if no request is active, we are not throttled, and there are pending requests to send, do so
	if (this->request_outstanding_ == false &&
		now - this->last_transmit_ >= this->request_throttle_ &&
		(this->read_queue_.size() > 0 || this->write_queue_.size() > 0)) {
		// this will do any desired logging
		this->send_next_request_frame_();
		this->request_outstanding_ = true;
		this->last_transmit_ = now;
		this->last_receive_ = now;
		this->raw_data_index_ = 0;
		return;
	}

	// if a request is active but we have passed the response timeout period and no more data is available, abandon the request
	if (this->request_outstanding_ == true &&
		now - this->last_receive_ >= this->response_timeout_ &&
		this->available() == 0) {
		if (this->raw_data_index_ > 0) {
			std::string str(this->raw_data_, this->raw_data_ + this->raw_data_index_ + 1);
			ESP_LOGW(TAG, "Response frame timeout for request %s after %i ms, partial frame: %s", this->last_request_description.c_str(), now - this->last_receive_, str.c_str());
		}
		else {
			ESP_LOGW(TAG, "Response frame timeout for request %s after %i ms, no valid data received", this->last_request_description.c_str(), now - this->last_receive_);
		}
		request_outstanding_ = false;
		this->raw_data_index_ = 0;
		return;
	}

	// if no data or no request outstanding, nothing to do
	if (this->available() == 0 ||
		this->request_outstanding_ == false) {
		return;
	}

	// reset timer since we're actively receiving
	this->last_receive_ = now;

	while (this->available() != 0) {
		this->read_byte(&this->raw_data_[this->raw_data_index_]);

		// is the SOI marker present at byte 0?
		if (this->raw_data_index_ == 0 && this->raw_data_[this->raw_data_index_] != '~') {
			ESP_LOGV(TAG, "Response frame does not begin with '~', actual: 0x%02X = '%c'", this->raw_data_[this->raw_data_index_], this->raw_data_[this->raw_data_index_]);
			request_outstanding_ = false;
			this->raw_data_index_ = 0;
			return;
		}

		// is this the end of a frame? process it
		if (this->raw_data_[this->raw_data_index_] == '\r') {
			// this will do any desired logging
			this->process_response_frame_(this->raw_data_, this->raw_data_index_ + 1);
			request_outstanding_ = false;
			this->raw_data_index_ = 0;
			return;
		}

		// did we run out of buffer before EOI?
		if (this->raw_data_index_ + 1 >= this->max_data_len_) {
			std::string str(this->raw_data_, this->raw_data_ + this->raw_data_index_ + 1);
			ESP_LOGV(TAG, "Response frame exceeds maximum supported length, last request was '%s', incomplete response frame: %s", this->last_request_description.c_str(), str.c_str());
			request_outstanding_ = false;
			this->raw_data_index_ = 0;
			return;
		}

		this->raw_data_index_++;
	}
}

// pops the next item off of this->command_queue_, generates and dispatches a request frame, and sets up this->next_response_handler_
void PaceBms::send_next_request_frame_() {

	if (read_queue_.empty() && write_queue_.empty()) {
		ESP_LOGE(TAG, "command queue empty on send_next_request_frame");
		return;
	}

	// always process writes first
	PaceBms::command_item* command;
	if (!write_queue_.empty()) {
		command = write_queue_.front();
		write_queue_.pop_front();
	}
	else {
		command = read_queue_.front();
		read_queue_.pop();
	}

	// process_response_frame_ will call this on the next frame received
	this->next_response_handler_ = command->process_response_frame_;
	// saved for logging
	this->last_request_description = command->description_;

	std::vector<uint8_t> request;
	if (false == command->create_request_frame_(request)) {
		ESP_LOGE(TAG, "Error creating '%s' request frame", command->description_.c_str());
		return;
	}

	ESP_LOGD(TAG, "Sending '%s' request", command->description_.c_str());
#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_VERY_VERBOSE
	{
		std::string str(request.data(), request.data() + request.size());
		ESP_LOGVV(TAG, "Request frame: %s", str.c_str());
	}
#endif

	if (this->flow_control_pin_ != nullptr)
		this->flow_control_pin_->digital_write(true);
	this->write_array(request.data(), request.size());
	// if flow control is required (rs485 does read+write on the same differential pair) then I don't see any other option than to block on flush()
	// if using rs232, a flow control pin should not be assigned in yaml in order to avoid this block
	if (this->flow_control_pin_ != nullptr) {
		this->flush();
		this->flow_control_pin_->digital_write(false);
	}

	delete(command);
}

// calls this->next_response_handler_ (set up from the previously dispatched command_queue_ item)
void PaceBms::process_response_frame_(uint8_t* frame_bytes, uint8_t frame_length) {
	ESP_LOGV(TAG, "Processing response frame for '%s' request", this->last_request_description.c_str());
#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_VERY_VERBOSE
	{
		std::string str(frame_bytes, frame_bytes + frame_length);
		ESP_LOGVV(TAG, "Response frame: %s", str.c_str());
	}
#endif

	std::vector<uint8_t> response(frame_bytes, frame_bytes + frame_length);

	if (next_response_handler_ != nullptr)
		next_response_handler_(response);
	else
		ESP_LOGE(TAG, "Response frame received but no response handler set");

	// this request/response pair is complete, any additional frames received will not be expected and should not be processed until the next command queue pop / send
	next_response_handler_ = nullptr;
}

/*
* read/write response frame received handlers, called via next_response_handler_ from process_response_frame
*/

void PaceBms::handle_read_analog_information_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV25::AnalogInformation analog_information;
	bool result = this->pace_bms_v25_->ProcessReadAnalogInformationResponse(this->address_, response, analog_information);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}

	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->analog_information_callbacks_v25_.size(); i++) {
		analog_information_callbacks_v25_[i](analog_information);
	}
}

void PaceBms::handle_read_status_information_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV25::StatusInformation status_information;
	bool result = this->pace_bms_v25_->ProcessReadStatusInformationResponse(this->address_, response, status_information);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}

	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->status_information_callbacks_v25_.size(); i++) {
		status_information_callbacks_v25_[i](status_information);
	}
}

void PaceBms::handle_read_hardware_version_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	std::string hardware_version;
	bool result = this->pace_bms_v25_->ProcessReadHardwareVersionResponse(this->address_, response, hardware_version);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}

	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->hardware_version_callbacks_v25_.size(); i++) {
		hardware_version_callbacks_v25_[i](hardware_version);
	}
}

void PaceBms::handle_read_serial_number_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	std::string serial_number;
	bool result = this->pace_bms_v25_->ProcessReadSerialNumberResponse(this->address_, response, serial_number);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}

	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->serial_number_callbacks_v25_.size(); i++) {
		serial_number_callbacks_v25_[i](serial_number);
	}
}

void PaceBms::handle_write_switch_command_response_v25(PaceBmsV25::SwitchCommand switch_command, std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	bool result = this->pace_bms_v25_->ProcessWriteSwitchCommandResponse(this->address_, switch_command, response);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' response", this->last_request_description.c_str());
		return;
	}
}

void PaceBms::handle_write_mosfet_switch_command_response_v25(PaceBmsV25::MosfetType type, PaceBmsV25::MosfetState state, std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	bool result = this->pace_bms_v25_->ProcessWriteMosfetSwitchCommandResponse(this->address_, type, state, response);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' response", this->last_request_description.c_str());
		return;
	}
}

void PaceBms::handle_write_shutdown_command_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	bool result = this->pace_bms_v25_->ProcessWriteShutdownCommandResponse(this->address_, response);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' response", this->last_request_description.c_str());
		return;
	}
}

void PaceBms::handle_read_protocols_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV25::Protocols protocols;
	bool result = this->pace_bms_v25_->ProcessReadProtocolsResponse(this->address_, response, protocols);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}
}

void PaceBms::handle_write_protocols_response_v25(PaceBmsV25::Protocols protocols, std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	bool result = this->pace_bms_v25_->ProcessWriteProtocolsResponse(this->address_, response);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' response", this->last_request_description.c_str());
		return;
	}
}

void PaceBms::handle_read_cell_over_voltage_configuration_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV25::CellOverVoltageConfiguration config;
	bool result = this->pace_bms_v25_->ProcessReadConfigurationResponse(this->address_, response, config);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}
	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->cell_over_voltage_configuration_callbacks_v25_.size(); i++) {
		cell_over_voltage_configuration_callbacks_v25_[i](config);
	}
}

void PaceBms::handle_read_pack_over_voltage_configuration_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV25::PackOverVoltageConfiguration config;
	bool result = this->pace_bms_v25_->ProcessReadConfigurationResponse(this->address_, response, config);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}
	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->pack_over_voltage_configuration_callbacks_v25_.size(); i++) {
		pack_over_voltage_configuration_callbacks_v25_[i](config);
	}
}

void PaceBms::handle_read_cell_under_voltage_configuration_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV25::CellUnderVoltageConfiguration config;
	bool result = this->pace_bms_v25_->ProcessReadConfigurationResponse(this->address_, response, config);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}
	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->cell_under_voltage_configuration_callbacks_v25_.size(); i++) {
		cell_under_voltage_configuration_callbacks_v25_[i](config);
	}
}

void PaceBms::handle_read_pack_under_voltage_configuration_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV25::PackUnderVoltageConfiguration config;
	bool result = this->pace_bms_v25_->ProcessReadConfigurationResponse(this->address_, response, config);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}
	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->pack_under_voltage_configuration_callbacks_v25_.size(); i++) {
		pack_under_voltage_configuration_callbacks_v25_[i](config);
	}
}

void PaceBms::handle_read_charge_over_current_configuration_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV25::ChargeOverCurrentConfiguration config;
	bool result = this->pace_bms_v25_->ProcessReadConfigurationResponse(this->address_, response, config);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}

	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->charge_over_current_configuration_callbacks_v25_.size(); i++) {
		charge_over_current_configuration_callbacks_v25_[i](config);
	}
}

void PaceBms::handle_read_discharge_over_current1_configuration_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV25::DischargeOverCurrent1Configuration config;
	bool result = this->pace_bms_v25_->ProcessReadConfigurationResponse(this->address_, response, config);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}
	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->discharge_over_current1_configuration_callbacks_v25_.size(); i++) {
		discharge_over_current1_configuration_callbacks_v25_[i](config);
	}
}

void PaceBms::handle_read_discharge_over_current2_configuration_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV25::DischargeOverCurrent2Configuration config;
	bool result = this->pace_bms_v25_->ProcessReadConfigurationResponse(this->address_, response, config);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}
	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->discharge_over_current2_configuration_callbacks_v25_.size(); i++) {
		discharge_over_current2_configuration_callbacks_v25_[i](config);
	}
}

void PaceBms::handle_read_short_circuit_protection_configuration_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV25::ShortCircuitProtectionConfiguration config;
	bool result = this->pace_bms_v25_->ProcessReadConfigurationResponse(this->address_, response, config);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}
	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->short_circuit_protection_configuration_callbacks_v25_.size(); i++) {
		short_circuit_protection_configuration_callbacks_v25_[i](config);
	}
}

void PaceBms::handle_read_cell_balancing_configuration_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV25::CellBalancingConfiguration config;
	bool result = this->pace_bms_v25_->ProcessReadConfigurationResponse(this->address_, response, config);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}
	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->cell_balancing_configuration_callbacks_v25_.size(); i++) {
		cell_balancing_configuration_callbacks_v25_[i](config);
	}
}

void PaceBms::handle_read_sleep_configuration_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV25::SleepConfiguration config;
	bool result = this->pace_bms_v25_->ProcessReadConfigurationResponse(this->address_, response, config);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}
	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->sleep_configuration_callbacks_v25_.size(); i++) {
		sleep_configuration_callbacks_v25_[i](config);
	}
}

void PaceBms::handle_read_full_charge_low_charge_configuration_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV25::FullChargeLowChargeConfiguration config;
	bool result = this->pace_bms_v25_->ProcessReadConfigurationResponse(this->address_, response, config);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}
	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->full_charge_low_charge_configuration_callbacks_v25_.size(); i++) {
		full_charge_low_charge_configuration_callbacks_v25_[i](config);
	}
}

void PaceBms::handle_read_charge_and_discharge_over_temperature_configuration_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV25::ChargeAndDischargeOverTemperatureConfiguration config;
	bool result = this->pace_bms_v25_->ProcessReadConfigurationResponse(this->address_, response, config);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}
	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->charge_and_discharge_over_temperature_configuration_callbacks_v25_.size(); i++) {
		charge_and_discharge_over_temperature_configuration_callbacks_v25_[i](config);
	}
}

void PaceBms::handle_read_charge_and_discharge_under_temperature_configuration_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV25::ChargeAndDischargeUnderTemperatureConfiguration config;
	bool result = this->pace_bms_v25_->ProcessReadConfigurationResponse(this->address_, response, config);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}
	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->charge_and_discharge_under_temperature_configuration_callbacks_v25_.size(); i++) {
		charge_and_discharge_under_temperature_configuration_callbacks_v25_[i](config);
	}
}

void PaceBms::handle_write_configuration_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	bool result = this->pace_bms_v25_->ProcessWriteConfigurationResponse(this->address_, response);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' response", this->last_request_description.c_str());
		return;
	}
}

void PaceBms::handle_read_system_datetime_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV25::DateTime dt;
	bool result = this->pace_bms_v25_->ProcessReadSystemDateTimeResponse(this->address_, response, dt);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}
	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->system_datetime_callbacks_v25_.size(); i++) {
		system_datetime_callbacks_v25_[i](dt);
	}
}

void PaceBms::handle_read_mosfet_over_temperature_configuration_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV25::MosfetOverTemperatureConfiguration config;
	bool result = this->pace_bms_v25_->ProcessReadConfigurationResponse(this->address_, response, config);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}
	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->mosfet_over_temperature_configuration_callbacks_v25_.size(); i++) {
		mosfet_over_temperature_configuration_callbacks_v25_[i](config);
	}
}

void PaceBms::handle_read_environment_over_under_temperature_configuration_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV25::EnvironmentOverUnderTemperatureConfiguration config;
	bool result = this->pace_bms_v25_->ProcessReadConfigurationResponse(this->address_, response, config);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}
	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->environment_over_under_temperature_configuration_callbacks_v25_.size(); i++) {
		environment_over_under_temperature_configuration_callbacks_v25_[i](config);
	}
}

void PaceBms::handle_write_system_datetime_response_v25(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	bool result = this->pace_bms_v25_->ProcessWriteSystemDateTimeResponse(this->address_, response);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' response", this->last_request_description.c_str());
		return;
	}
}


void PaceBms::handle_read_analog_information_response_v20(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV20::AnalogInformation analog_information;
	bool result = this->pace_bms_v20_->ProcessReadAnalogInformationResponse(this->address_, response, analog_information);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}

	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->analog_information_callbacks_v20_.size(); i++) {
		analog_information_callbacks_v20_[i](analog_information);
	}
}

void PaceBms::handle_read_status_information_response_v20(std::vector<uint8_t>& response) {
	ESP_LOGD(TAG, "Processing '%s' response", this->last_request_description.c_str());

	PaceBmsV20::StatusInformation status_information;
	bool result = this->pace_bms_v20_->ProcessReadStatusInformationResponse(this->address_, response, status_information);
	if (result == false) {
		ESP_LOGE(TAG, "Unable to decode '%s' request", this->last_request_description.c_str());
		return;
	}

	// dispatch to any child components that registered for a callback with us
	for (int i = 0; i < this->status_information_callbacks_v20_.size(); i++) {
		status_information_callbacks_v20_[i](status_information);
	}
}

/*
* these are called from from user-settable child sensors to set BMS state
*/

// helper for when multiple callbacks come due to fast UX interaction
void PaceBms::write_queue_push_back_with_deduplication(command_item* item) {
	auto iter = std::find_if(this->write_queue_.begin(), this->write_queue_.end(),
		[&item](const command_item* test) -> bool {
			return test->description_ == item->description_;
		});

	if (iter != this->write_queue_.end()) {
		std::swap((*iter), item);
		delete item;
	}
	else {
		this->write_queue_.push_back(item);
	}
}

void PaceBms::set_switch_state_v25(PaceBmsV25::SwitchCommand state) {
	command_item* item = new command_item;

	// this is just to generate the text
	switch (state) {
	case PaceBmsV25::SC_DisableBuzzer:
	case PaceBmsV25::SC_EnableBuzzer:
		item->description_ = std::string("write buzzer alarm state ") + (state == PaceBmsV25::SC_EnableBuzzer ? "ON" : "OFF");
		break;
	case PaceBmsV25::SC_DisableLedWarning:
	case PaceBmsV25::SC_EnableLedWarning:
		item->description_ = std::string("write led alarm state ") + (state == PaceBmsV25::SC_EnableLedWarning ? "ON" : "OFF");
		break;
	case PaceBmsV25::SC_DisableChargeCurrentLimiter:
	case PaceBmsV25::SC_EnableChargeCurrentLimiter:
		item->description_ = std::string("write charge current limiter state ") + (state == PaceBmsV25::SC_EnableChargeCurrentLimiter ? "ON" : "OFF");
		break;
	case PaceBmsV25::SC_SetChargeCurrentLimiterCurrentLimitHighGear:
	case PaceBmsV25::SC_SetChargeCurrentLimiterCurrentLimitLowGear:
		item->description_ = std::string("write charge current limiter gear ") + (state == PaceBmsV25::SC_SetChargeCurrentLimiterCurrentLimitHighGear ? "High" : "Low");
	}

	ESP_LOGV(TAG, "Queueing write command '%s'", item->description_.c_str());
	item->create_request_frame_ = [this, state](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateWriteSwitchCommandRequest(this->address_, state, request); };
	item->process_response_frame_ = [this, state](std::vector<uint8_t>& response) -> void { this->handle_write_switch_command_response_v25(state, response); };
	write_queue_push_back_with_deduplication(item);
	ESP_LOGV(TAG, "Write commands queued: %i", write_queue_.size());
}

void PaceBms::set_mosfet_state_v25(PaceBmsV25::MosfetType type, PaceBmsV25::MosfetState state) {
	command_item* item = new command_item;

	// this is just to generate the text
	switch (type) {
	case PaceBmsV25::MT_Charge:
		item->description_ = std::string("write charge mosfet state ") + (state == PaceBmsV25::MS_Open ? "Open" : "Closed");
		break;
	case PaceBmsV25::MT_Discharge:
		item->description_ = std::string("write discharge mosfet state ") + (state == PaceBmsV25::MS_Open ? "Open" : "Closed");
		break;
	}

	ESP_LOGV(TAG, "Queueing write command '%s'", item->description_.c_str());
	item->create_request_frame_ = [this, type, state](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateWriteMosfetSwitchCommandRequest(this->address_, type, state, request); };
	item->process_response_frame_ = [this, type, state](std::vector<uint8_t>& response) -> void { this->handle_write_mosfet_switch_command_response_v25(type, state, response); };
	write_queue_push_back_with_deduplication(item);
	ESP_LOGV(TAG, "Write commands queued: %i", write_queue_.size());
}

void PaceBms::send_shutdown_v25() {
	command_item* item = new command_item;

	item->description_ = std::string("write shutdown");
	ESP_LOGV(TAG, "Queueing write command '%s'", item->description_.c_str());
	item->create_request_frame_ = [this](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateWriteShutdownCommandRequest(this->address_, request); };
	item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_write_shutdown_command_response_v25(response); };
	write_queue_push_back_with_deduplication(item);
	ESP_LOGV(TAG, "Write commands queued: %i", write_queue_.size());
}

void PaceBms::set_protocols_v25(PaceBmsV25::Protocols& protocols) {
	command_item* item = new command_item;

	item->description_ = std::string("write protocols");
	ESP_LOGV(TAG, "Queueing write command '%s'", item->description_.c_str());
	item->create_request_frame_ = [this, protocols](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateWriteProtocolsRequest(this->address_, protocols, request); };
	item->process_response_frame_ = [this, protocols](std::vector<uint8_t>& response) -> void { this->handle_write_protocols_response_v25(protocols, response); };
	write_queue_push_back_with_deduplication(item);
	ESP_LOGV(TAG, "Write commands queued: %i", write_queue_.size());
}

void PaceBms::set_cell_over_voltage_configuration_v25(PaceBmsV25::CellOverVoltageConfiguration& config) {
	command_item* item = new command_item;

	item->description_ = std::string("write cell over voltage configuration");
	ESP_LOGV(TAG, "Queueing write command '%s'", item->description_.c_str());
	item->create_request_frame_ = [this, config](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateWriteConfigurationRequest(this->address_, config, request); };
	item->process_response_frame_ = [this, config](std::vector<uint8_t>& response) -> void { this->handle_write_configuration_response_v25(response); };
	write_queue_push_back_with_deduplication(item);
	ESP_LOGV(TAG, "Write commands queued: %i", write_queue_.size());
}

void PaceBms::set_pack_over_voltage_configuration_v25(PaceBmsV25::PackOverVoltageConfiguration& config) {
	command_item* item = new command_item;

	item->description_ = std::string("write pack over voltage configuration");
	ESP_LOGV(TAG, "Queueing write command '%s'", item->description_.c_str());
	item->create_request_frame_ = [this, config](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateWriteConfigurationRequest(this->address_, config, request); };
	item->process_response_frame_ = [this, config](std::vector<uint8_t>& response) -> void { this->handle_write_configuration_response_v25(response); };
	write_queue_push_back_with_deduplication(item);
	ESP_LOGV(TAG, "Write commands queued: %i", write_queue_.size());
}

void PaceBms::set_cell_under_voltage_configuration_v25(PaceBmsV25::CellUnderVoltageConfiguration& config) {
	command_item* item = new command_item;

	item->description_ = std::string("write cell under voltage configuration");
	ESP_LOGV(TAG, "Queueing write command '%s'", item->description_.c_str());
	item->create_request_frame_ = [this, config](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateWriteConfigurationRequest(this->address_, config, request); };
	item->process_response_frame_ = [this, config](std::vector<uint8_t>& response) -> void { this->handle_write_configuration_response_v25(response); };
	write_queue_push_back_with_deduplication(item);
	ESP_LOGV(TAG, "Write commands queued: %i", write_queue_.size());
}

void PaceBms::set_pack_under_voltage_configuration_v25(PaceBmsV25::PackUnderVoltageConfiguration& config) {
	command_item* item = new command_item;

	item->description_ = std::string("write pack under voltage configuration");
	ESP_LOGV(TAG, "Queueing write command '%s'", item->description_.c_str());
	item->create_request_frame_ = [this, config](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateWriteConfigurationRequest(this->address_, config, request); };
	item->process_response_frame_ = [this, config](std::vector<uint8_t>& response) -> void { this->handle_write_configuration_response_v25(response); };
	write_queue_push_back_with_deduplication(item);
	ESP_LOGV(TAG, "Write commands queued: %i", write_queue_.size());
}

void PaceBms::set_charge_over_current_configuration_v25(PaceBmsV25::ChargeOverCurrentConfiguration& config) {
	command_item* item = new command_item;

	item->description_ = std::string("write charge over current configuration");
	ESP_LOGV(TAG, "Queueing write command '%s'", item->description_.c_str());
	item->create_request_frame_ = [this, config](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateWriteConfigurationRequest(this->address_, config, request); };
	item->process_response_frame_ = [this, config](std::vector<uint8_t>& response) -> void { this->handle_write_configuration_response_v25(response); };
	write_queue_push_back_with_deduplication(item);
	ESP_LOGV(TAG, "Write commands queued: %i", write_queue_.size());
}

void PaceBms::set_discharge_over_current1_configuration_v25(PaceBmsV25::DischargeOverCurrent1Configuration& config) {
	command_item* item = new command_item;

	item->description_ = std::string("write discharge over current 1 configuration");
	ESP_LOGV(TAG, "Queueing write command '%s'", item->description_.c_str());
	item->create_request_frame_ = [this, config](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateWriteConfigurationRequest(this->address_, config, request); };
	item->process_response_frame_ = [this, config](std::vector<uint8_t>& response) -> void { this->handle_write_configuration_response_v25(response); };
	write_queue_push_back_with_deduplication(item);
	ESP_LOGV(TAG, "Write commands queued: %i", write_queue_.size());
}

void PaceBms::set_discharge_over_current2_configuration_v25(PaceBmsV25::DischargeOverCurrent2Configuration& config) {
	command_item* item = new command_item;

	item->description_ = std::string("write discharge over current 2 configuration");
	ESP_LOGV(TAG, "Queueing write command '%s'", item->description_.c_str());
	item->create_request_frame_ = [this, config](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateWriteConfigurationRequest(this->address_, config, request); };
	item->process_response_frame_ = [this, config](std::vector<uint8_t>& response) -> void { this->handle_write_configuration_response_v25(response); };
	write_queue_push_back_with_deduplication(item);
	ESP_LOGV(TAG, "Write commands queued: %i", write_queue_.size());
}

void PaceBms::set_short_circuit_protection_configuration_v25(PaceBmsV25::ShortCircuitProtectionConfiguration& config) {
	command_item* item = new command_item;

	item->description_ = std::string("write short circuit protection configuration");
	ESP_LOGV(TAG, "Queueing write command '%s'", item->description_.c_str());
	item->create_request_frame_ = [this, config](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateWriteConfigurationRequest(this->address_, config, request); };
	item->process_response_frame_ = [this, config](std::vector<uint8_t>& response) -> void { this->handle_write_configuration_response_v25(response); };
	write_queue_push_back_with_deduplication(item);
	ESP_LOGV(TAG, "Write commands queued: %i", write_queue_.size());
}

void PaceBms::set_cell_balancing_configuration_v25(PaceBmsV25::CellBalancingConfiguration& config) {
	command_item* item = new command_item;

	item->description_ = std::string("write cell balancing configuration");
	ESP_LOGV(TAG, "Queueing write command '%s'", item->description_.c_str());
	item->create_request_frame_ = [this, config](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateWriteConfigurationRequest(this->address_, config, request); };
	item->process_response_frame_ = [this, config](std::vector<uint8_t>& response) -> void { this->handle_write_configuration_response_v25(response); };
	write_queue_push_back_with_deduplication(item);
	ESP_LOGV(TAG, "Write commands queued: %i", write_queue_.size());
}

void PaceBms::set_sleep_configuration_v25(PaceBmsV25::SleepConfiguration& config) {
	command_item* item = new command_item;

	item->description_ = std::string("write sleep configuration");
	ESP_LOGV(TAG, "Queueing write command '%s'", item->description_.c_str());
	item->create_request_frame_ = [this, config](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateWriteConfigurationRequest(this->address_, config, request); };
	item->process_response_frame_ = [this, config](std::vector<uint8_t>& response) -> void { this->handle_write_configuration_response_v25(response); };
	write_queue_push_back_with_deduplication(item);
	ESP_LOGV(TAG, "Write commands queued: %i", write_queue_.size());
}

void PaceBms::set_full_charge_low_charge_configuration_v25(PaceBmsV25::FullChargeLowChargeConfiguration& config) {
	command_item* item = new command_item;

	item->description_ = std::string("write full charge low charge configuration");
	ESP_LOGV(TAG, "Queueing write command '%s'", item->description_.c_str());
	item->create_request_frame_ = [this, config](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateWriteConfigurationRequest(this->address_, config, request); };
	item->process_response_frame_ = [this, config](std::vector<uint8_t>& response) -> void { this->handle_write_configuration_response_v25(response); };
	write_queue_push_back_with_deduplication(item);
	ESP_LOGV(TAG, "Write commands queued: %i", write_queue_.size());
}

void PaceBms::set_charge_and_discharge_over_temperature_configuration_v25(PaceBmsV25::ChargeAndDischargeOverTemperatureConfiguration& config) {
	command_item* item = new command_item;

	item->description_ = std::string("write charge and discharge over temperature configuration");
	ESP_LOGV(TAG, "Queueing write command '%s'", item->description_.c_str());
	item->create_request_frame_ = [this, config](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateWriteConfigurationRequest(this->address_, config, request); };
	item->process_response_frame_ = [this, config](std::vector<uint8_t>& response) -> void { this->handle_write_configuration_response_v25(response); };
	write_queue_push_back_with_deduplication(item);
	ESP_LOGV(TAG, "Write commands queued: %i", write_queue_.size());
}

void PaceBms::set_charge_and_discharge_under_temperature_configuration_v25(PaceBmsV25::ChargeAndDischargeUnderTemperatureConfiguration& config) {
	command_item* item = new command_item;

	item->description_ = std::string("write charge and discharge under temperature configuration");
	ESP_LOGV(TAG, "Queueing write command '%s'", item->description_.c_str());
	item->create_request_frame_ = [this, config](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateWriteConfigurationRequest(this->address_, config, request); };
	item->process_response_frame_ = [this, config](std::vector<uint8_t>& response) -> void { this->handle_write_configuration_response_v25(response); };
	write_queue_push_back_with_deduplication(item);
	ESP_LOGV(TAG, "Write commands queued: %i", write_queue_.size());
}

void PaceBms::set_mosfet_over_temperature_configuration_v25(PaceBmsV25::MosfetOverTemperatureConfiguration& config) {
	command_item* item = new command_item;

	item->description_ = std::string("write mosfet over temperature configuration");
	ESP_LOGV(TAG, "Queueing write command '%s'", item->description_.c_str());
	item->create_request_frame_ = [this, config](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateWriteConfigurationRequest(this->address_, config, request); };
	item->process_response_frame_ = [this, config](std::vector<uint8_t>& response) -> void { this->handle_write_configuration_response_v25(response); };
	write_queue_push_back_with_deduplication(item);
	ESP_LOGV(TAG, "Write commands queued: %i", write_queue_.size());
}

void PaceBms::set_environment_over_under_temperature_configuration_v25(PaceBmsV25::EnvironmentOverUnderTemperatureConfiguration& config) {
	command_item* item = new command_item;

	item->description_ = std::string("write environment over under temperature configuration");
	ESP_LOGV(TAG, "Queueing write command '%s'", item->description_.c_str());
	item->create_request_frame_ = [this, config](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateWriteConfigurationRequest(this->address_, config, request); };
	item->process_response_frame_ = [this, config](std::vector<uint8_t>& response) -> void { this->handle_write_configuration_response_v25(response); };
	write_queue_push_back_with_deduplication(item);
	ESP_LOGV(TAG, "Write commands queued: %i", write_queue_.size());
}

void PaceBms::set_system_datetime_v25(PaceBmsV25::DateTime& dt) {
	command_item* item = new command_item;

	item->description_ = std::string("write system date/time");
	ESP_LOGV(TAG, "Queueing write command '%s'", item->description_.c_str());
	item->create_request_frame_ = [this, dt](std::vector<uint8_t>& request) -> bool { return this->pace_bms_v25_->CreateWriteSystemDateTimeRequest(this->address_, dt, request); };
	item->process_response_frame_ = [this](std::vector<uint8_t>& response) -> void { this->handle_write_system_datetime_response_v25(response); };
	write_queue_push_back_with_deduplication(item);
	ESP_LOGV(TAG, "Write commands queued: %i", write_queue_.size());
}

}  // namespace pace_bms
}  // namespace esphome
