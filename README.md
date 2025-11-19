## Arduino Cloud Online IDE Version (Highly recommended)
This project provides an environment and configuration tailored for the Arduino Cloud Online IDE.
This release is based on version 2.3.4, with layout optimizations specifically for the Arduino Cloud Online IDE and several known issues fixed.
The list of fixes is documented here.

Note: Please use LVGL version 8.3.10.

🔗 **Arduino Cloud Public Link**  
https://app.arduino.cc/sketches/8d74d1a2-4075-44a0-b8a9-69095435e36b?nav=Libraries&view-mode=preview


# FAPs GUI-Design Project
This is a 10 ECTS project carried out during the WS2025 semester at FAU, under the topic "User Interface and Control Software for an Automated Test Bench for the Analysis of Biofilm Formation" in the FAPS department.

The project focuses on developing an Arduino-based automated test platform designed to simulate urination cycles and monitor biofilm formation on artificial urethral sphincter implants. The system includes a touch-based user interface and hardware control components such as a peristaltic pump, UV sterilization, and temperature sensing.


# User Interface and Control Software for a Automated Test Bench for the Analysis of Biofilm Formation
Stress urinary incontinence is a common condition that affects around 3,000,000 people in Germany alone. Current artificial urinary sphincters have high failure rates, require an invasive procedure with hospitalization, are unintuitive to use and are usually unsuitable for women, although they make up the majority of the affected population. To solve these problems, a new type of intraurethral sphincter prosthesis is developed. The purely mechanical design enables intuitive control of micturition, similar to normal bladder emptying, while continence is maintained during events such as coughing, laughing or lifting heavy objects. Due to its small size, the implant is suitable for both male and female patients and can be used by patients immediately after minimally invasive outpatient implantation.

The aim of this work is to develop the control software and the user interface for an automated test bench for analyzing biofilm formation on the implant in contact with urine. This should make it possible to carry out long-term tests on the implant.

The following tasks must be completed as part of the work:<br>
-Development of a touch screen UI for the Arduino Giga Display Shield, which allows to set how many micturition cycles per day should be conducted, how long each cycle will be (in seconds) and to start the system. Additionally it has to show the ambient temperature, the selected parameters and the elapsed time and micturition cycles.<br>
-Development of a control software, which uses the Arduino Motor Shield to control a UCV sterilization LED and a peristaltic pump and a BMP280 sensor to measure the ambient temperature.<br>
-Evaluation of the system (ease of use, functionality)
