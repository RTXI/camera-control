/*
 * Copyright (C) 2006 Weill Medical College of Cornell University
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <default_gui_model.h>
#include <QtGui>

class CameraCtl : public DefaultGUIModel {

	public:
		CameraCtl(void);
		virtual ~CameraCtl(void);
		virtual void execute(void);

	protected:
		virtual void update(DefaultGUIModel::update_flags_t);

	private:
		int NumFrames;
		int FrameCount;
		int FrameRate;

		double duration;
		int durationCount;
		double LampDelay;
		int LampDelayCount;
		double CameraDelay;
		int CameraDelayCount;
		double CameraDuration;
		int CameraDurationCount;
		long long count;
		int DelayCount;
		double OffFrameDelay;
		int OffFrameStartCount;
};
