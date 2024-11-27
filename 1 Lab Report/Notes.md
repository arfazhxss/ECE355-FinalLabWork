# Notes

## Why is there a limitation on detecting a certain range of frequencies?

### **System Clock and Timer Counts**
- The timer counts system clock cycles between two rising edges of the signal. For a 48 MHz clock, each clock cycle is **20.83 ns**.
- The number of counts, \( N \), recorded by the timer for one signal period (\( T \)) is:
  \[
  N = T \times f_{\text{clock}}
  \]
  Where:
  - \( T \) is the period of the signal (in seconds).
  - \( f_{\text{clock}} \) is the system clock frequency (48 MHz in your case).

### **High-Frequency Signals**
- For a high-frequency signal like **500 kHz**, the period (\( T \)) is:
  \[
  T = \frac{1}{f_{\text{signal}}} = \frac{1}{500,000} = 2 \, \mu \text{s}
  \]
- The number of timer counts in this period is:
  \[
  N = T \times f_{\text{clock}} = 2 \times 10^{-6} \times 48 \times 10^6 = 96 \, \text{counts}
  \]
- With just **96 counts**, there’s little room for error. A single missed or additional clock cycle due to noise, jitter, or interrupt latency would introduce significant inaccuracy. For example:
  - Missing **1 clock cycle** would change \( N \) from 96 to 95, introducing a ~1% error in frequency measurement.

### **Accuracy Issues at Small \( T \)**
1. **Quantization Error:**
   - The resolution of the timer is 1 clock cycle. For small periods, the difference between successive counts (\( N \) and \( N+1 \)) corresponds to a larger fractional error in period and, hence, frequency.

2. **Interrupt Overhead:**
   - If the interrupt handling introduces even a tiny delay (e.g., 1–2 clock cycles), it skews the count for \( T \), especially for small \( T \).

### **Comparison to Low-Frequency Signals**
- For a low-frequency signal like **1 kHz**, the period is:
  \[
  T = \frac{1}{1,000} = 1 \, \text{ms}
  \]
- The number of counts becomes:
  \[
  N = T \times f_{\text{clock}} = 1 \times 10^{-3} \times 48 \times 10^6 = 48,000 \, \text{counts}
  \]
- With 48,000 counts, the impact of missing or additional clock cycles is negligible compared to high-frequency signals, making low-frequency measurements more accurate.

### **Conclusion**
Your understanding is correct: as the period gets smaller (higher frequencies), the number of counts decreases, making the measurement more prone to inaccuracies due to quantization error, interrupt latency, and signal noise. These factors collectively contribute to the anomalies you observe around 500 kHz. Including this reasoning in your report will effectively explain why your system has such limitations.
