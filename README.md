# Mesh visualizer for material assignment

Existing research recognises the critical role played by material assignment on 3D surfaces. A number of proposed solutions which are making inroads recently, consist of methods based on algorithms designed to provide material suggestions relying on labelling systems or requiring heavy workload from the end user. The goal of this dissertation is to explore a data-driven approach, yet independent from either labels or comparisons with other 3D objects, for automatically assigning materials on 3D surfaces. Geometry-based descriptors such as Fourier Descriptors and Shape Context Descriptors are exploited to extract feature sets from 3D objects and images stored in a local database in order to measure their similarity. Eventually, similarity measurements are performed to recognise and retrieve materials information from the most similar figures that are subsequently automatically applied to the 3D object.

We implemented a software aimed to support the end user in this automatic material assignment process with the help of a three-dimensional environment and with a particular focus on reliability, effectiveness and efficiency. Further improvements to the user interface and the reliability of the outcomes in specific contexts are still necessary.

Developed on Ubuntu 16.04 with Qt

The basic mesh loader is inspired from : https://github.com/BennyQBD/ModernOpenGLTutorial
