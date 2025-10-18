<template>
  <div class="calendar-widget">
    <h3>Select your pickup and return dates on the calendar</h3>
    <VueDatePicker
      :value="selectedDates"
      range
      inline
      auto-apply
      :enable-time-picker="false"
      :disabled-dates="disabledDates"
      :min-date="currentDateString"
      :start-date="currentDateString"
      :month="currentMonth"
      :year="currentYear"
      :model-config="modelConfig"
      @update:model-value="handleDateUpdate"
    />
    <p v-if="dateValidationError" class="error">{{ dateValidationError }}</p>
    <p v-if="isSelectionComplete" class="success">
      Dates and times successfully selected!
    </p>
    <div v-if="isValidRentalDates" class="time-pickers">
      <div class="time-picker">
        <label>Pickup Time</label>
        <VueDatePicker
          v-model="localPickupTime"
          time-picker
          :is-24="false"
          :enable-seconds="false"
          format="hh:mm a"
          text-input
          auto-apply
          :model-config="{ type: 'string', mask: 'hh:mm a' }"
          :start-time="parseTime(localPickupTime)"
          @update:model-value="(time) => handleTimeUpdate(time, true)"
        />
        <input
          v-if="localPickupTime && !isInputVisible"
          :value="localPickupTime"
          type="text"
          placeholder="Select pickup time"
          class="fallback-input"
          @input="handleFallbackInput($event, true)"
        />
      </div>
      <div class="time-picker">
        <label>Drop-off Time</label>
        <VueDatePicker
          v-model="localDropoffTime"
          time-picker
          :is-24="false"
          :enable-seconds="false"
          format="hh:mm a"
          text-input
          auto-apply
          :model-config="{ type: 'string', mask: 'hh:mm a' }"
          :start-time="parseTime(localDropoffTime)"
          @update:model-value="(time) => handleTimeUpdate(time, false)"
        />
        <input
          v-if="localDropoffTime && !isInputVisible"
          :value="localDropoffTime"
          type="text"
          placeholder="Select drop-off time"
          class="fallback-input"
          @input="handleFallbackInput($event, false)"
        />
      </div>
    </div>
  </div>
</template>

<script>
import VueDatePicker from "@vuepic/vue-datepicker";
import "@vuepic/vue-datepicker/dist/main.css";

export default {
  name: "RentalDateTimePicker",
  components: { VueDatePicker },
  props: {
    item: { type: Object, required: true },
    bookings: { type: Array, default: () => [] },
    apiBaseUrl: { type: String, required: true },
    selectedDates: { type: Array, default: () => [] },
    pickupTime: { type: String, default: "09:00 AM" },
    dropoffTime: { type: String, default: "05:00 PM" },
  },
  data() {
    const currentDate = new Date();
    return {
      dateValidationError: "",
      isInputVisible: true,
      isSelectionComplete: false,
      localPickupTime: this.pickupTime,
      localDropoffTime: this.dropoffTime,
      currentDateString: `${currentDate.getFullYear()}-${String(
        currentDate.getMonth() + 1
      ).padStart(2, "0")}-${String(currentDate.getDate()).padStart(2, "0")}`,
      currentMonth: currentDate.getMonth(),
      currentYear: currentDate.getFullYear(),
      modelConfig: { type: "string", mask: "YYYY-MM-DD" },
    };
  },
  watch: {
    pickupTime(newVal) {
      this.localPickupTime = newVal;
    },
    dropoffTime(newVal) {
      this.localDropoffTime = newVal;
    },
  },
  computed: {
    isValidRentalDates() {
      if (!this.selectedDates[0] || !this.selectedDates[1]) {
        this.dateValidationError = "Please select a start and end date";
        this.$emit("update:date-validation-error", this.dateValidationError);
        return false;
      }
      const start = new Date(this.selectedDates[0]);
      const end = new Date(this.selectedDates[1]);
      const today = new Date();
      today.setHours(0, 0, 0, 0);
      if (start <= today) {
        this.dateValidationError = "Start date must be after today";
        this.$emit("update:date-validation-error", this.dateValidationError);
        return false;
      }
      if (end < start) {
        this.dateValidationError = "End date must be after start date";
        this.$emit("update:date-validation-error", this.dateValidationError);
        return false;
      }
      const durationDays = Math.ceil((end - start) / (1000 * 60 * 60 * 24)) + 1;
      if (durationDays < this.item.minDays) {
        this.dateValidationError = `Rental must be at least ${this.item.minDays} days`;
        this.$emit("update:date-validation-error", this.dateValidationError);
        return false;
      }
      if (durationDays > this.item.maxDays) {
        this.dateValidationError = `Rental cannot exceed ${this.item.maxDays} days`;
        this.$emit("update:date-validation-error", this.dateValidationError);
        return false;
      }
      const allDisabledDates = [
        ...(this.item.unavailableDates || []).map((range) => ({
          startDate: range.startDate,
          endDate: range.endDate,
        })),
        ...this.bookings.map((booking) => ({
          startDate: this.toDateString(booking.startDate),
          endDate: this.toDateString(booking.endDate),
        })),
      ];
      for (const unavailable of allDisabledDates) {
        const unavailableStart = new Date(unavailable.startDate);
        const unavailableEnd = new Date(unavailable.endDate);
        unavailableEnd.setDate(unavailableEnd.getDate() + 1);
        if (
          isNaN(unavailableStart.getTime()) ||
          isNaN(unavailableEnd.getTime())
        ) {
          console.error(`Invalid date range: ${JSON.stringify(unavailable)}`);
          continue;
        }
        if (start < unavailableEnd && end >= unavailableStart) {
          this.dateValidationError = `Selected dates overlap with unavailable or booked dates (${this.formatDate(
            unavailable.startDate
          )} to ${this.formatDate(unavailable.endDate)})`;
          this.$emit("update:date-validation-error", this.dateValidationError);
          return false;
        }
      }
      this.dateValidationError = "";
      this.$emit("update:date-validation-error", "");
      return true;
    },
    isValidTimes() {
      if (!this.localPickupTime || !this.localDropoffTime) {
        this.dateValidationError = "Please select pickup and drop-off times";
        this.$emit("update:date-validation-error", this.dateValidationError);
        return false;
      }
      if (this.selectedDates[0] === this.selectedDates[1]) {
        const startTime = new Date(
          `2000-01-01 ${this.convertTo24Hour(this.localPickupTime)}`
        );
        const endTime = new Date(
          `2000-01-01 ${this.convertTo24Hour(this.localDropoffTime)}`
        );
        if (startTime >= endTime) {
          this.dateValidationError =
            "Drop-off time must be after pickup time on the same day";
          this.$emit("update:date-validation-error", this.dateValidationError);
          return false;
        }
      }
      this.dateValidationError = "";
      this.$emit("update:date-validation-error", "");
      return true;
    },
    disabledDates() {
      return (date) => {
        const today = new Date();
        today.setHours(0, 0, 0, 0);
        if (date <= today) {
          return true; // Disable today and earlier dates
        }
        const allDisabledDates = [
          ...(this.item.unavailableDates || []).map((range) => ({
            startDate: range.startDate,
            endDate: range.endDate,
          })),
          ...this.bookings.map((booking) => ({
            startDate: this.toDateString(booking.startDate),
            endDate: this.toDateString(booking.endDate),
          })),
        ];
        return allDisabledDates.some((unavailable) => {
          try {
            const start = new Date(unavailable.startDate);
            const end = new Date(unavailable.endDate);
            end.setDate(end.getDate() + 1);
            if (isNaN(start.getTime()) || isNaN(end.getTime())) {
              console.error(
                `Invalid date range: ${JSON.stringify(unavailable)}`
              );
              return false;
            }
            return date >= start && date < end;
          } catch (e) {
            console.error(
              `Error processing date range: ${JSON.stringify(unavailable)}`,
              e
            );
            return false;
          }
        });
      };
    },
  },
  mounted() {
    this.$nextTick(() => {
      const inputs = this.$el.querySelectorAll(".dp__input");
      this.isInputVisible =
        inputs.length > 0 && inputs[0].offsetParent !== null;
      console.log("VueDatePicker inputs visible:", this.isInputVisible);
      console.log("Inputs found:", inputs.length);
      console.log("Input elements:", inputs);
      console.log("Item unavailableDates:", this.item.unavailableDates);
      console.log("Bookings:", this.bookings);
    });
  },
  methods: {
    handleDateUpdate(dates) {
      console.log("handleDateUpdate triggered with:", dates);
      const formattedDates = dates.map((date) => {
        if (date instanceof Date) {
          const year = date.getFullYear();
          const month = String(date.getMonth() + 1).padStart(2, "0");
          const day = String(date.getDate()).padStart(2, "0");
          return `${year}-${month}-${day}`;
        }
        return date;
      });
      this.$emit("update:selected-dates", formattedDates);
      console.log("Date picker updated:", formattedDates);

      this.isSelectionComplete = false;

      if (
        formattedDates.length === 2 &&
        formattedDates[0] &&
        formattedDates[1]
      ) {
        console.log("Checking validity for auto-selection:", {
          isValidRentalDates: this.isValidRentalDates,
          isValidTimes: this.isValidTimes,
        });
        if (this.isValidRentalDates && this.isValidTimes) {
          this.isSelectionComplete = true;
          this.$emit("dates-selected", {
            selectedDates: formattedDates,
            pickupTime: this.localPickupTime,
            dropoffTime: this.localDropoffTime,
          });
          console.log("Dates automatically selected:", {
            selectedDates: formattedDates,
            pickupTime: this.localPickupTime,
            dropoffTime: this.localDropoffTime,
          });
        } else {
          console.log("Date selection invalid:", this.dateValidationError);
        }
      }
    },
    handleTimeUpdate(time, isPickup) {
      console.log("handleTimeUpdate triggered with:", { time, isPickup });
      let timeStr;
      if (
        time &&
        typeof time === "object" &&
        time.hours != null &&
        time.minutes != null
      ) {
        const hours = time.hours % 12 || 12;
        const ampm = time.hours >= 12 ? "PM" : "AM";
        timeStr = `${String(hours).padStart(2, "0")}:${String(
          time.minutes
        ).padStart(2, "0")} ${ampm}`;
      } else {
        timeStr = time || (isPickup ? "09:00 AM" : "05:00 PM");
      }
      if (isPickup) {
        this.localPickupTime = timeStr;
      } else {
        this.localDropoffTime = timeStr;
      }
      this.$emit(
        isPickup ? "update:pickup-time" : "update:dropoff-time",
        timeStr
      );
      console.log("Time picker updated:", {
        timeStr,
        pickup: this.localPickupTime,
        dropoff: this.localDropoffTime,
      });

      this.isSelectionComplete = false;

      if (
        this.selectedDates.length === 2 &&
        this.selectedDates[0] &&
        this.selectedDates[1]
      ) {
        console.log("Checking validity after time update:", {
          isValidRentalDates: this.isValidRentalDates,
          isValidTimes: this.isValidTimes,
        });
        if (this.isValidRentalDates && this.isValidTimes) {
          this.isSelectionComplete = true;
          this.$emit("dates-selected", {
            selectedDates: this.selectedDates,
            pickupTime: this.localPickupTime,
            dropoffTime: this.localDropoffTime,
          });
          console.log("Dates automatically selected after time update:", {
            selectedDates: this.selectedDates,
            pickupTime: this.localPickupTime,
            dropoffTime: this.localDropoffTime,
          });
        } else {
          console.log("Time update invalid:", this.dateValidationError);
        }
      }
    },
    handleFallbackInput(event, isPickup) {
      const timeStr = event.target.value;
      if (isPickup) {
        this.localPickupTime = timeStr;
        this.$emit("update:pickup-time", timeStr);
      } else {
        this.localDropoffTime = timeStr;
        this.$emit("update:dropoff-time", timeStr);
      }
      this.handleTimeUpdate(timeStr, isPickup);
    },
    parseTime(timeStr) {
      if (!timeStr) return { hours: 9, minutes: 0 };
      const [time, ampm] = timeStr.split(" ");
      let [hours, minutes] = time.split(":").map(Number);
      if (ampm === "PM" && hours !== 12) hours += 12;
      if (ampm === "AM" && hours === 12) hours = 0;
      return { hours, minutes };
    },
    convertTo24Hour(timeStr) {
      if (!timeStr) return null;
      const [time, ampm] = timeStr.split(" ");
      let [hours, minutes] = time.split(":").map(Number);
      if (ampm === "PM" && hours !== 12) hours += 12;
      if (ampm === "AM" && hours === 12) hours = 0;
      return `${String(hours).padStart(2, "0")}:${String(minutes).padStart(
        2,
        "0"
      )}`;
    },
    toDateString(dateStr) {
      if (!dateStr) return null;
      const [year, month, day] = dateStr.split("T")[0].split("-").map(Number);
      const date = new Date(year, month - 1, day);
      if (isNaN(date.getTime())) {
        console.error(`Invalid date string: ${dateStr}`);
        return null;
      }
      return `${date.getFullYear()}-${String(date.getMonth() + 1).padStart(
        2,
        "0"
      )}-${String(date.getDate()).padStart(2, "0")}`;
    },
    formatDate(date) {
      if (!date) return "N/A";
      return new Date(date).toLocaleString();
    },
  },
};
</script>

<style scoped>
.calendar-widget {
  margin: 20px 0;
}
.time-pickers {
  display: flex;
  gap: 20px;
  margin-top: 10px;
}
.time-picker {
  display: flex;
  flex-direction: column;
  gap: 5px;
}
:deep(.dp__time_input) {
  display: none;
}
.error {
  color: #dc3545;
  font-size: 0.9em;
}
.success {
  color: #28a745;
  font-size: 0.9em;
}
.fallback-input {
  padding: 5px;
  border: 1px solid #ccc;
  border-radius: 4px;
  width: 100px;
  font-size: 14px;
}
:deep(.dp__input) {
  padding: 5px;
  border: 1px solid #ccc;
  border-radius: 4px;
  width: 100px;
  font-size: 14px;
  box-sizing: border-box;
  display: block !important;
  visibility: visible !important;
}
:deep(.dp__input_wrap) {
  position: relative;
}
:deep(.dp__cell_disabled) {
  background-color: #dc3545 !important;
  color: white !important;
  position: relative;
}
:deep(.dp__cell_disabled::after) {
  content: "X";
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  font-weight: bold;
  color: white;
}
:deep(.dp__range_start, .dp__range_end, .dp__range_between) {
  background-color: #007bff !important;
  color: white !important;
}
</style>