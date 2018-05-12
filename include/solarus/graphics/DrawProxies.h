#pragma once

#include "solarus/graphics/SurfacePtr.h"
#include "solarus/core/Rectangle.h"
#include "solarus/graphics/BlendMode.h"

#include <array>

namespace Solarus {

struct DrawProxy;

struct DrawInfos {
  //Replace params proxy
  DrawInfos(const Rectangle& region,const Point& dst_position,
            BlendMode blend_mode, uint8_t opacity,
            const DrawProxy& proxy):
    region(region),dst_position(dst_position),
    blend_mode(blend_mode), opacity(opacity),
    proxy(proxy) {}
  DrawInfos(const DrawInfos& other, const DrawProxy& proxy) :
    DrawInfos(other.region,other.dst_position,other.blend_mode,other.opacity,proxy) {}
  //TODO more helper constructors
  const Rectangle& region;
  const Point& dst_position;
  BlendMode blend_mode;
  uint8_t   opacity;
  const DrawProxy& proxy;
};

/**
 * @brief The DrawProxy is used to draw with various modifiers
 */
struct DrawProxy {
  virtual void draw(Surface& dst_surface, Surface& src_surface,const DrawInfos& params) const = 0;
  virtual bool is_terminal() const {return false;}
};

template<std::size_t N>
struct DrawProxyPipeline {
  DrawProxyPipeline(const std::array<DrawProxy,N>& proxies) : proxies(proxies) {
    assert(proxies.size() > 0);
    assert(proxies.back().is_terminal()); //Make sure last proxy is terminal
  }

  void draw(Surface& dst_surface, Surface& src_surface, const DrawInfos& params) const override {
    proxies.front().draw(dst_surface,src_surface,DrawInfos(params,PipeProxy{*this,1}));
  }
private:
  struct PipeProxy {
    const DrawProxyPipeline& pipeline;
    size_t index;
    void draw(Surface& dst_surface, Surface& src_surface, const DrawInfos& params) const override {
      if(index < pipeline.proxies.size())
        pipeline.proxies.at(index).draw(dst_surface,src_surface,DrawInfos(params,PipeProxy{pipeline,index+1}));
      else
        params.proxy.draw(dst_surface,src_surface,params); //TODO check if this happen... should not
    }
  };
  const std::array<DrawProxy,N>& proxies;
};

}
// DRAWPROXIES_H
