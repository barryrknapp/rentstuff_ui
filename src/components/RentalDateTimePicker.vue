<template>
  <div class="calendar-widget">
    <h3>Select Rental Dates</h3>
    <VueDatePicker
      :value="selectedDates"
      range
      inline
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
    <div v-if="isValidRentalDates" class="time-pickers">
      <div class="time-picker">
        <label>Pickup Time</label>
        <VueDatePicker
          :value="pickupTime"
          time-picker
          :is-24="false"
          :enable-seconds="false"
          format="hh:mm a"
          text-input
          :model-config="{ type: 'string', mask: 'hh:mm a' }"
          @update:model-value="(time) => handleTimeUpdate(time, true)"
        />
        <input
          v-if="pickupTime && !isInputVisible"
          :value="pickupTime"
          type="text"
          placeholder="Select pickup time"
          class="fallback-input"
          @input="$emit('update:pickup-time', $event.target.value)"
        />
      </div>
      <div class="time-picker">
        <label>Drop-off Time</label>
        <VueDatePicker
          :value="dropoffTime"
          time-picker
          :is-24="false"
          :enable-seconds="false"
          format="hh:mm a"
          text-input
          :model-config="{ type: 'string', mask: 'hh:mm a' }"
          @update:model-value="(time) => handleTimeUpdate(time, false)"
        />
        <input
          v-if="dropoffTime && !isInputVisible"
          :value="dropoffTime"
          type="text"
          placeholder="Select drop-off time"
          class="fallback-input"
          @input="$emit('update:dropoff-time', $event.target.value)"
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
    item: {
      type: Object,
      required: true,
    },
    bookings: {
      type: Array,
      default: () => [],
    },
    apiBaseUrl: {
      type: String,
      required: true,
    },
    selectedDates: {
      type: Array,
      default: () => [],
    },
    pickupTime: {
      type: String,
      default: "09:00 AM",
    },
    dropoffTime: {
      type: String,
      default: "09:00 AM",
    },
  },
  data() {
    const currentDate = new Date();
    return {
      dateValidationError: "",
      isInputVisible: true,
      currentDateString: `${currentDate.getFullYear()}-${String(
        currentDate.getMonth() + 1
      ).padStart(2, "0")}-${String(currentDate.getDate()).padStart(2, "0")}`,
      currentMonth: currentDate.getMonth(),
      currentYear: currentDate.getFullYear(),
      modelConfig: {
        type: "string",
        mask: "YYYY-MM-DD",
      },
    };
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
      if (start < today) {
        this.dateValidationError = "Start date must be today or later";
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
        unavailableEnd.setDate(unavailableEnd.getDate() + 1); // Include end date
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
      if (!this.pickupTime || !this.dropoffTime) {
        this.dateValidationError = "Please select pickup and drop-off times";
        this.$emit("update:date-validation-error", this.dateValidationError);
        return false;
      }
      if (this.selectedDates[0] === this.selectedDates[1]) {
        const startTime = new Date(
          `2000-01-01 ${this.convertTo24Hour(this.pickupTime)}`
        );
        const endTime = new Date(
          `2000-01-01 ${this.convertTo24Hour(this.dropoffTime)}`
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
        //console.log("Disabled date ranges:", allDisabledDates);
        const disabled = allDisabledDates.some((unavailable) => {
          try {
            const start = new Date(unavailable.startDate);
            const end = new Date(unavailable.endDate);
            end.setDate(end.getDate() + 1); // Include end date
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
        //if (disabled) {
        //console.log(`Disabled date: ${date.toISOString().split("T")[0]}`);
        //}
        return disabled;
      };
    },
  },
  mounted() {
    this.$nextTick(() => {
      const inputs = this.$el.querySelectorAll(".dp__input");
      this.isInputVisible =
        inputs.length > 0 && inputs[0].offsetParent !== null;
      console.log("VueDatePicker inputs visible:", this.isInputVisible);
      console.log("Item unavailableDates:", this.item.unavailableDates);
      console.log("Bookings:", this.bookings);
      //const testDates = ["2025-10-17", "2025-10-18"];
      //testDates.forEach((testDate) => {
      //  console.log(
      //    `Test date (${testDate}) disabled:`,
      //    this.disabledDates(new Date(testDate))
      //  );
      //});
    });
  },
  methods: {
    handleDateUpdate(dates) {
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
    },
    handleTimeUpdate(time, isPickup) {
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
        timeStr = time || "09:00 AM";
      }
      this.$emit(
        isPickup ? "update:pickup-time" : "update:dropoff-time",
        timeStr
      );
      console.log("Time picker updated:", {
        time,
        pickup: this.pickupTime,
        dropoff: this.dropoffTime,
      });
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
.error {
  color: #dc3545;
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