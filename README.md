# Motor Test Bench

This project consists of a test bench for small DC motors. The main objective of the system is to enable the characterization of motors with unknown data. The system performs measurements of rotational speed and torque of the motor under test while gradually varying the load on the motor shaft.

The speed is measured using a quadrature encoder. The encoder disc is coupled to the motor shaft and as it rotates, it generates pulses in an optical switch. Counting these pulses, when related to elapsed time, allows for the calculation of the rotational speed of the shaft.

The torque is indirectly measured. A lever arm is pressed against the motor shaft by a spring, whose tension can be varied, thereby altering the load on the shaft. The rotation of the motor shaft causes this lever arm to press against a load cell, which measures the applied force. By relating the measured force to the length of the lever arm, the torque is calculated.

During a test, the load on the motor shaft is gradually increased automatically. A stepper motor provides linear advancement under the spring, increasing its tension and consequently the load.

With the obtained data, it is possible to generate "torque vs. speed" and "power vs. speed" graphs of the motor, as well as identify parameters such as maximum speed, holding torque, and maximum power.

The current version of the project is still a prototype. In the next version, all electronics will be integrated, and new functionalities will be developed. The system will include measurement of the motor's input current, allowing for the calculation of its input power and consequently, its efficiency.