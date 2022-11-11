#include<plugin.h>
#include<OpcodeBase.hpp>
#include <modload.h>
#include <libgamepad.hpp>

std::shared_ptr<gamepad::hook> h (nullptr);

static void make_hook()
{
    if(h.get() == nullptr)
    {
        h = gamepad::hook::make();
        h->query_devices();
    }
}

struct gamepad_detect : csnd::Plugin<3, 0>
{
  int init()
  {
      make_hook();
      return OK;
  }
  int deinit()
  {
      return OK;
  }

  int kperf()
  {
      for(auto & it : h->get_devices())
      {
          if(it->update() == gamepad::update_result::NONE)
              continue;
          outargs[0] = (MYFLT)it->get_index();
          if(it->last_axis_event()->time > it->last_button_event()->time) {
            outargs[1] = 0;
            outargs[2] = (MYFLT)it->last_axis_event()->vc;
            outargs[3] = (MYFLT)it->last_axis_event()->virtual_value;
          } else {
            outargs[1] = 1;
            outargs[2] = (MYFLT)it->last_button_event()->vc;
            outargs[3] = (MYFLT)it->last_button_event()->virtual_value;
          }
          break;
      }

      return OK;
  }
};


struct gamepad_track_device : public csnd::Plugin<2, 1>
{
    int init()
    {
        make_hook();
        _index = (size_t)inargs[0];
        if(_index >= h->get_devices().size())
        {
            csound->init_error("Gamepad -> This device does not exist");
            return NOTOK;
        }
        dev_ptr = h->get_devices()[_index];
        return OK;
    }

    int kperf()
    {
        if(_index >= h->get_devices().size())
        {
            csound->perf_error("Gamepad -> This device does not exist", this);
            return NOTOK;
        }
        if(dev_ptr->update() == gamepad::update_result::NONE)
            return OK;

        if(dev_ptr->last_axis_event()->time > dev_ptr->last_button_event()->time) {
            outargs[0] = (MYFLT)dev_ptr->last_axis_event()->vc;
            outargs[1] = (MYFLT)dev_ptr->last_axis_event()->virtual_value;
        } else {
            outargs[0] = (MYFLT)dev_ptr->last_button_event()->native_id;
            outargs[1] = (MYFLT)dev_ptr->last_button_event()->virtual_value;
        }
        return OK;
    }

    std::shared_ptr<gamepad::device> dev_ptr;
    size_t _index;
};


struct gamepad_track_control : public csnd::Plugin<1, 2>
{
    int init()
    {
        make_hook();
        _index = (size_t)inargs[0];
        _control = (size_t)inargs[1];
        if(_index >= h->get_devices().size())
        {
            csound->init_error("Gamepad -> This device does not exist");
            return NOTOK;
        }
        dev_ptr = h->get_devices()[_index];
        return OK;
    }

    int kperf()
    {
        if(_index >= h->get_devices().size())
        {
            csound->perf_error("Gamepad -> This device does not exist", this);
            return NOTOK;
        }
        if(dev_ptr->update() == gamepad::update_result::NONE)
            return OK;

        if(dev_ptr->last_axis_event()->vc == _control)
            outargs[0] = dev_ptr->last_axis_event()->virtual_value;
        else if(dev_ptr->last_button_event()->vc == _control)
            outargs[0] = dev_ptr->last_button_event()->virtual_value;

        return OK;
    }

    std::shared_ptr<gamepad::device> dev_ptr;
    size_t _index, _control;
};

void csnd::on_load(Csound *csound) {

     /* Outargs :
     * - Device index
     * - Button or axis index - according to libgamepad, these indexes are plateform independant (and are not starting from 1).
     * - Value (0-1 range). For Axis, 0.5 is center of axis (so 0.0 and 1.0 are bounds)
     */
    csnd::plugin<gamepad_detect>(csound, "gamepad", "kkk", "", csnd::thread::ik);


    /* Inarg :
     * - index : device index
     *
     * Outargs :
     * - Button or axis index - according to libgamepad, these indexes are plateform independant (and are not starting from 1).
     * - Value (0-1 range). For Axis, 0.5 is center of axis (so 0.0 and 1.0 are bounds)
     */
    csnd::plugin<gamepad_track_device>(csound, "gamepad_track_device", "kk", "i", csnd::thread::ik);


    /* Inarg :
     * - index : device index
     * - Control (axis or button) index - according to libgamepad, these indexes are plateform independant (and are not starting from 1). It should be determined with gamepad or gamepad_track_device opcodes.
     *
     * Outargs :
     * - Value (0-1 range). For Axis, 0.5 is center of axis (so 0.0 and 1.0 are bounds)
     */
    csnd::plugin<gamepad_track_control>(csound, "gamepad_track_control", "k", "ii", csnd::thread::ik);
}
