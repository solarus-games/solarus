#pragma once

#include "solarus/graphics/SurfacePtr.h"
#include "solarus/core/Rectangle.h"
#include "solarus/graphics/BlendMode.h"

#include <array>

namespace Solarus {

struct DrawProxy;

struct DrawInfos {
  //Replace params proxy
  constexpr DrawInfos(const Rectangle& region,const Point& dst_position,
            BlendMode blend_mode, uint8_t opacity,
            const DrawProxy& proxy):
    region(region),dst_position(dst_position),
    blend_mode(blend_mode), opacity(opacity),
    proxy(proxy) {}
  constexpr DrawInfos(const DrawInfos& other, const DrawProxy& proxy) :
    DrawInfos(other.region,other.dst_position,other.blend_mode,other.opacity,proxy) {}
  constexpr DrawInfos(const DrawInfos &other, const Rectangle& region,
            const Point& dst_position) :
    DrawInfos(region,dst_position,other.blend_mode,other.opacity,other.proxy) {}
  constexpr DrawInfos(const DrawInfos &other, const Point& dst_position) :
    DrawInfos(other.region,dst_position,other.blend_mode,other.opacity,other.proxy) {}
  constexpr DrawInfos(const DrawInfos& other,uint8_t opacity):
    DrawInfos(other.region,other.dst_position,other.blend_mode,opacity,other.proxy) {}
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
  virtual void draw(Surface& dst_surface, const Surface& src_surface,const DrawInfos& params) const = 0;
  virtual bool is_terminal() const {return false;}
};

template<std::size_t N>
struct DrawProxyChain : DrawProxy {
  using Proxies = std::array<std::reference_wrapper<const DrawProxy>,N>;
  constexpr DrawProxyChain(const Proxies& proxies) : proxies(proxies) {
  }

  void draw(Surface& dst_surface, const Surface& src_surface, const DrawInfos& params) const override {
    proxies.front().get().draw(dst_surface,src_surface,DrawInfos(params,Link{*this,1}));
  }
private:
  struct Link : DrawProxy {
    constexpr Link(const DrawProxyChain& pipeline, size_t index) :
      pipeline(pipeline), index(index) {}
    void draw(Surface& dst_surface, const Surface& src_surface, const DrawInfos& params) const override {
      //if(index < pipeline.proxies.size())
        pipeline.proxies.at(index).get().draw(dst_surface,src_surface,DrawInfos(params,Link{pipeline,index+1}));
      //else
        //params.proxy.draw(dst_surface,src_surface,params); //TODO check if this happen... should not
    }
  private:
    const DrawProxyChain& pipeline;
    size_t index;
  };
  const Proxies& proxies;
};

template<std::size_t N>
DrawProxyChain<N> create_draw_pipeline(const std::array<DrawProxy,N>& proxies) {
  return DrawProxyChain<N>(proxies);
}

}
// DRAWPROXIES_H
