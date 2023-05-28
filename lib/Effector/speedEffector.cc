#include "Effector/speedEffector.hh"
#include "utils/justiceNumber.hh"

template <typename SamplePoint>
static SamplePoint interpolate(const SamplePoint &a, const SamplePoint &b,
                               double ratio) {
  return a + (b - a) * ratio;
}

// x: sample point position
template <typename SamplePoint>
static SamplePoint getValFromVector(const std::vector<SamplePoint> &v,
                                    double x) {
  auto i = static_cast<size_t>(x);
  auto ratio = x - i;
  if (i + 1 >= v.size()) {
    return v[i];
  }
  return interpolate(v[i], v[i + 1], ratio);
}

SpeedEffector::SpeedEffector(EffectorRef effector, double speed)
    : effector{std::move(effector)}, speed{speed} {}

Maybe<EffectorBuf> SpeedEffector::getData() {
  auto data = effector->getData();
  if (data) {
    auto kk = data.value();
    buf.clear();
    auto new_size = static_cast<size_t>(data->size() / speed);

    if (speed > 1) {
      auto segmentSize = static_cast<size_t>(128 / speed);
      auto dataSize = data->size();
      buf.reserve(new_size);
      for (size_t i = 0; i < dataSize; i++) {
        bool addToNew = i % 128 <= segmentSize;
        if (addToNew) {
          buf.push_back(getValFromVector(*data, i));
        }
      }
      return buf;
    }

    buf.reserve(new_size);
    for (size_t i = 0; i < new_size; i++) {
      buf.push_back(getValFromVector(*data, i * speed));
    }
  }
  return buf;
}