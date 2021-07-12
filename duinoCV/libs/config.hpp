#ifndef ESP32CAM_CONFIG_HPP
#define ESP32CAM_CONFIG_HPP

#include "internal/pins.hpp"

namespace detail {

/** \brief Convert JPEG quality from 0-100 scale to 63-0 scale.
 */
int
convertJpegQuality(int quality);

} // namespace detail

/** \brief Camera configuration.
 */
class Config
{
public:
  Config();

  ~Config();

  Config&
  setResolution();

  /** \brief Set number of frame buffers.
   *  \param n >=1, having more frame buffers allows better streaming fps.
   */
  Config&
  setBufferCount(int n);

/* \brief Change pixel format to RGB565.*/
  Config&
  setPins(const Pins& pins);
  
  /** \brief Change pixel format to RGB565.
   */
  Config&
  setRgb();

  /** \brief Change pixel format to YUV422.
   */
  Config&
  setYuv();

  /** \brief Change pixel format to grayscale.
   */
  Config& setGrayscale();
  void init();

  /** \brief Change pixel format to JPEG.
   *  \param quality JPEG quality between 0 (worst) and 100 (best).
   */
  Config&
  setJpeg(int quality);

private:
  class CameraConfigT; ///< camera_config_t
  CameraConfigT* m_cfg;

  friend class CameraClass;
};


#endif // ESP32CAM_CONFIG_HPP
