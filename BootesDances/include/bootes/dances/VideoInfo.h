#ifndef BOOTESDANCES_STAGE_VIDEOINFO_H
#define BOOTESDANCES_STAGE_VIDEOINFO_H

class VideoInfo
{
public:
   __int64 size_100ns;
   __int64 tpf_100ns;
   int width, height;

   inline VideoInfo() { };
   inline VideoInfo(const VideoInfo& r) { operator=(r); };
   VideoInfo& operator=(const VideoInfo& r) {
      size_100ns = r.size_100ns;
      tpf_100ns = r.tpf_100ns;
      width = r.width;
      height = r.height;
      return *this;
   }
};

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
