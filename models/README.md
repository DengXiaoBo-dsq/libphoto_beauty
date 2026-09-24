# Model layout

Suggested production layout:

models/
  face/
    detector/
    landmark/
    mesh/
  segmentation/
    person/
    skin/
    hair/
  enhancement/
    face_restore/
    denoise/
    super_resolution/

The C++ library should treat models as versioned assets:
  family / name / version / backend / precision

Examples of backend choices:
  CPU
  Vulkan
  NPU

Do not commit proprietary weights into the public source repository.
