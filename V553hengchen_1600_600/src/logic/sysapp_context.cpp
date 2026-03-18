/*
 * sysapp_context.cpp
 *
 *  Created on: 2023年5月26日
 *      Author: ZKSWE Develop Team
 */

#include "app/SysAppFactory.h"
#include "sysapp_context.h"
#include "common.h"

namespace app {

#define SHOW_CTRLBAR_TIMER     10000
#define HIDE_CTRLBAR_TIMER     10001
#define SHOW_FLOATWND_TIMER    10002
#define HIDE_FLOATWND_TIMER    10003

static app_register_timer_fun _s_reg_timer_fun;
static app_unregister_timer_fun _s_unreg_timer_fun;

static BaseApp *_s_ctrlbar;
static BaseApp *_s_floatwnd;

static void _reg_timer(int id, int time) {
	if (_s_reg_timer_fun) {
		_s_reg_timer_fun(id, time);
	}
}

void attach_timer(app_register_timer_fun reg, app_unregister_timer_fun unreg) {
	_s_reg_timer_fun = reg;
	_s_unreg_timer_fun = unreg;
}

bool on_timer(int id) {
	switch (id) {
	case SHOW_CTRLBAR_TIMER:
		if (!_s_ctrlbar) {
			_s_ctrlbar = SYSAPPFACTORY->create(APP_TYPE_SYS_CTRLBAR);
		}
		return true;

	case HIDE_CTRLBAR_TIMER:
		if (_s_ctrlbar) {
			delete _s_ctrlbar;
			_s_ctrlbar = NULL;
		}
		return true;

	case SHOW_FLOATWND_TIMER:
		if (!_s_floatwnd) {
			_s_floatwnd = SYSAPPFACTORY->create(APP_TYPE_SYS_FLOATWND);
		}
		return true;

	case HIDE_FLOATWND_TIMER:
		if (_s_floatwnd) {
			delete _s_floatwnd;
			_s_floatwnd = NULL;
		}
		return true;
	}

	return false;
}

void show_ctrlbar() {
	_reg_timer(SHOW_CTRLBAR_TIMER, 0);
}

void hide_ctrlbar() {
	_reg_timer(HIDE_CTRLBAR_TIMER, 0);
}

bool is_show_ctrlbar() {
	return _s_ctrlbar ? _s_ctrlbar->isShow() : false;
}

bool is_hit_ctrlbar(int x, int y) {
	return _s_ctrlbar ? _s_ctrlbar->getPosition().isHit(x, y) : false;
}

void show_floatwnd() {
	_reg_timer(SHOW_FLOATWND_TIMER, 0);
}

void hide_floatwnd() {
	_reg_timer(HIDE_FLOATWND_TIMER, 0);
}

bool is_show_floatwnd() {
	return _s_floatwnd ? _s_floatwnd->isShow() : false;
}

bool is_hit_floatwnd(int x, int y) {
	return _s_floatwnd ? _s_floatwnd->getPosition().isHit(x, y) : false;
}

}
