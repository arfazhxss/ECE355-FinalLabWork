# Notes

## Why is there a limitation on detecting a certain range of frequencies?
[Copied from ChatGPT](https://chatgpt.com/share/6746d4db-1a58-8013-9cf1-db1083aea2c7)

### **System Clock and Timer Counts**
- The timer counts system clock cycles between two rising edges of the signal. For a 48 MHz clock, each clock cycle is **20.83 ns**.
- The number of counts, \( N \), recorded by the timer for one signal period (\( T \)) is:
  \[
  N = T \times f_{\text{clock}}
  \]
  Where:
  - \( T \) is the period of the signal (in seconds).
  - \( f_{\text{clock}} \) is the system clock frequency (48 MHz in your case).

### **High-Frequency Signal: 600 kHz**
- **Period (\( T \))** of the signal:
  \[
  T = \frac{1}{f_{\text{signal}}} = \frac{1}{600,000} = 1.6667 \, \mu\text{s}
  \]

- **Number of Timer Counts (\( N \))**:
  \[
  N = T \times f_{\text{clock}} = 1.6667 \times 10^{-6} \times 48 \times 10^6 = 80 \, \text{counts}
  \]

---

### **Analysis**
1. **Low Count, High Error Sensitivity:**
   - With only **80 counts**, a missed or extra clock cycle introduces significant error. For instance:
     - Missing **1 clock cycle** changes \( N \) to 79, resulting in:
       \[
       \text{Frequency error} = \frac{f_{\text{clock}}}{N} - \frac{f_{\text{clock}}}{N-1}
       \]
       \[
       \text{Error} = \frac{48,000,000}{80} - \frac{48,000,000}{79} \approx 600,000 - 607,595 = 7,595 \, \text{Hz}
       \]
       This is an **error of ~1.27%** just from missing 1 clock cycle.

2. **Interrupt Overhead:**
   - The interrupt handling time must be significantly shorter than \( T = 1.6667 \, \mu\text{s} \). If it’s comparable, the system might miss edges entirely.

3. **Jitter and Noise:**
   - At such high frequencies, any signal noise or jitter can cause additional rising edges to be detected erroneously or edges to be missed altogether, introducing inaccuracies.

---

### **Comparison to Lower Frequencies**
- For a lower frequency like **1 kHz** (\( T = 1 \, \text{ms} \), \( N = 48,000 \)):
  - Missing 1 clock cycle has negligible impact:
    \[
    \text{Frequency error} \approx \frac{48,000,000}{48,000} - \frac{48,000,000}{47,999} \approx 1,000 - 999.98 = 0.02 \, \text{Hz}
    \]
    This is an **error of 0.002%**, much smaller than for 600 kHz.

---
