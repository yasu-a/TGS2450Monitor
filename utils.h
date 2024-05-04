#ifndef __UTILS_HEADER__
#define __UTILS_HEADER__

class IntervalTimer {
  const unsigned long interval_ms;
  unsigned long last_mtime_ms = 0;

public:
  IntervalTimer(const unsigned long interval_ms)
    : interval_ms(interval_ms) {}

  bool check_and_update() {
    unsigned long current_time_ms = millis();
    if (current_time_ms - this->last_mtime_ms < this->interval_ms) {
      return false;
    }
    this->last_mtime_ms = current_time_ms;
    return true;
  }
};

template<typename T> class SampleLPF {
  const T alpha;
  const T rescaler_thresh;  // must be > 1
  T x = 0;

public:
  SampleLPF(const T alpha, const T rescaler_thresh)
    : alpha(alpha), rescaler_thresh(rescaler_thresh) {}

  bool is_rescalable_sample(const T x_sampled) const {
    T scale = this->x / x_sampled;
    return scale > this->rescaler_thresh || 1 / scale > this->rescaler_thresh;
  }

  void append_sample(const T x_sampled) {
    if (this->is_rescalable_sample(x_sampled)) {
      this->x = x_sampled;
    } else {
      this->x = this->x * this->alpha + x_sampled * ((T)1 - this->alpha);
    }
  }

  T get_value() const {
    return this->x;
  }
};

template<typename T> class SampleAverager {
  const int n_samples;
  T value = 0;
  unsigned int count = 0;

public:
  SampleAverager(const int n_samples)
    : n_samples(n_samples) {}

  void clear() {
    this->value = 0;
    this->count = 0;
  }

  unsigned int get_n_samples() const {
    return this->n_samples;
  }

  unsigned int get_count() const {
    return count;
  }

  bool is_full() const {
    return this->count >= this->n_samples;
  }

  void append_sample(const T value) {
    if (this->is_full()) {
      return;
    }
    this->value += value;
    this->count++;
  }

  T get_value() {
    return this->value / (T)this->count;
  }
};

#endif /* __UTILS_HEADER__ */