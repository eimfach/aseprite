// Aseprite
// Copyright (C) 2018  David Capello
//
// This program is distributed under the terms of
// the End-User License Agreement for Aseprite.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "app/cmd/set_frame_duration.h"
#include "app/script/luacpp.h"
#include "app/tx.h"
#include "doc/frame.h"
#include "doc/sprite.h"

namespace app {
namespace script {

using namespace doc;

namespace {

struct FrameObj {
  Sprite* sprite;
  frame_t frame;
  FrameObj(Sprite* sprite, frame_t frame)
    : sprite(sprite),
      frame(frame) {
  }
  FrameObj(const FrameObj&) = delete;
  FrameObj& operator=(const FrameObj&) = delete;
};

int Frame_gc(lua_State* L)
{
  get_obj<FrameObj>(L, 1)->~FrameObj();
  return 0;
}

int Frame_eq(lua_State* L)
{
  const auto a = get_obj<FrameObj>(L, 1);
  const auto b = get_obj<FrameObj>(L, 2);
  lua_pushboolean(L,
                  (a->sprite == b->sprite &&
                   a->frame == b->frame));
  return 1;
}

int Frame_get_sprite(lua_State* L)
{
  auto obj = get_obj<FrameObj>(L, 1);
  push_ptr<Sprite>(L, obj->sprite);
  return 1;
}

int Frame_get_frameNumber(lua_State* L)
{
  auto obj = get_obj<FrameObj>(L, 1);
  lua_pushinteger(L, obj->frame+1);
  return 1;
}

int Frame_get_duration(lua_State* L)
{
  auto obj = get_obj<FrameObj>(L, 1);
  lua_pushnumber(L, obj->sprite->frameDuration(obj->frame) / 1000.0);
  return 1;
}

int Frame_set_duration(lua_State* L)
{
  auto obj = get_obj<FrameObj>(L, 1);
  double duration = lua_tonumber(L, 2) * 1000.0;
  Tx tx;
  tx(new cmd::SetFrameDuration(obj->sprite, obj->frame, int(duration)));
  tx.commit();
  return 1;
}

const luaL_Reg Frame_methods[] = {
  { "__gc", Frame_gc },
  { "__eq", Frame_eq },
  { nullptr, nullptr }
};

const Property Frame_properties[] = {
  { "sprite", Frame_get_sprite, nullptr },
  { "frameNumber", Frame_get_frameNumber, nullptr },
  { "duration", Frame_get_duration, Frame_set_duration },
  { nullptr, nullptr, nullptr }
};

} // anonymous namespace

DEF_MTNAME(FrameObj);

void register_frame_class(lua_State* L)
{
  using Frame = FrameObj;
  REG_CLASS(L, Frame);
  REG_CLASS_PROPERTIES(L, Frame);
}

void push_sprite_frame(lua_State* L, Sprite* sprite, frame_t frame)
{
  push_new<FrameObj>(L, sprite, frame);
}

} // namespace script
} // namespace app
