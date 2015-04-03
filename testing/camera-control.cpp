/*
* Copyright (C) 2004 Boston University
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

#include <camera-control.h>

extern "C" Plugin::Object *createRTXIPlugin(void) {
	return new CameraCtl();
}

// inputs, outputs, parameters
static DefaultGUIModel::variable_t vars[] = {
	{
		"State",
		"Trigger state, =1 to start lamp",
		DefaultGUIModel::INPUT,
	},
	{
		"Lamp On",
		"0-5V",
		DefaultGUIModel::OUTPUT,
	},
	{
		"Camera On",
		"0-5V",
		DefaultGUIModel::OUTPUT,
	},    
	{
		"Lamp Duration",
		"Sec",
		DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
	},
	{
		"Lamp Delay",
		"Sec",
		DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
	},
	{
		"Camera Delay",
		"Sec",
		DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
	},    
	{
		"Off Frame Delay",
		"Sec",
		DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
	},
	{
		"Events",
		"Integer",
		DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER,
	},
	{
		"Frame Rate",
		"Frames/Sec",
		DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER,
	},

};

// some necessary variable
static size_t num_vars = sizeof(vars)/sizeof(DefaultGUIModel::variable_t);

// provides default values for paramters, calls update(INIT)
CameraCtl::CameraCtl(void) : DefaultGUIModel("Camera Control",::vars,::num_vars) {
	duration = 0.040;
	NumFrames = 1024;
	FrameRate = 25;
	CameraDelay = 0;
	LampDelay = 0;
	CameraDuration = 0.005;
	OffFrameDelay = 0.06;
	createGUI(vars, num_vars);
	update(INIT);
	count = 0;
	refresh();
	QTimer::singleShot(0, this, SLOT(resizeMe()));
}

CameraCtl::~CameraCtl(void) {}

// execute is run every time step
void CameraCtl::execute(void) {

	output(0) = 0;
	output(1) = 5;

	//Initialize system;
	if(FrameCount<0 || FrameCount>NumFrames) {
		if(FrameCount>NumFrames+26) return;
		if(count>0 && count<CameraDurationCount/2) {
			output(1) = 0;  // send state information as output
		}
		count++;
		if(count>CameraDurationCount) {
			count = 0;
			FrameCount++;
		}
		return;
	}	


	if(input(0)==1) {
		if(count<0 || count>(OffFrameStartCount+CameraDurationCount)) {
			//Do not start a new acquisition if the old one is not finished.
			count = 0;
			FrameCount++;
		}
	}


	//Determine signal to send to Lamp;
	//Do not turn on lamp for first 26 shots, thsese are not recorded by NeuroPlex software.
	DelayCount = count - LampDelayCount;
	if(DelayCount>0 && DelayCount<durationCount && FrameCount<=NumFrames) {
		output(0) = 10;  // send state information as output
	}


	//Determine signal to send to Camera;
	DelayCount = count - CameraDelayCount;

	if(DelayCount>0 && DelayCount<CameraDurationCount/2) {
		output(1) = 0 ;  // send state information as output
	}

	DelayCount = count - LampDelayCount - OffFrameStartCount;
	if(DelayCount>0 && DelayCount<durationCount && FrameCount>=0 && FrameCount<=NumFrames) {
		output(0) = 10;  // send state information as output
	}

	//Determine signal to send to Camera;
	DelayCount = count - CameraDelayCount-OffFrameStartCount;

	if(DelayCount>0 && DelayCount<CameraDurationCount/2) {
		output(1) = 0 ;  // send state information as output
	}

	count++;
}

void CameraCtl::update(DefaultGUIModel::update_flags_t flag) {
	switch(flag) {
	case INIT:
		setParameter("Lamp Duration",QString::number(duration));
		setParameter("Lamp Delay",QString::number(LampDelay));
		setParameter("Camera Delay",QString::number(CameraDelay));
		setParameter("Events", QString::number(NumFrames));
		setParameter("Frame Rate",QString::number(FrameRate));
		setParameter("Off Frame Delay",QString::number(OffFrameDelay));
		break;

	case MODIFY:
		duration = getParameter("Lamp Duration").toDouble();
		LampDelay = getParameter("Lamp Delay").toDouble();
		CameraDelay = getParameter("Camera Delay").toDouble();
		OffFrameDelay = getParameter("Off Frame Delay").toDouble();
		FrameRate = getParameter("Frame Rate").toInt();
		NumFrames =  getParameter("Events").toInt();
		break;

	case PAUSE:
		output(0) = 0;
		output(1) = 0;
		FrameCount = -26;
		break;

	default:
		break;
	}

	durationCount = (int)(duration/((double)RT::System::getInstance()->getPeriod()*1e-9));
	LampDelayCount = (int)(LampDelay/((double)RT::System::getInstance()->getPeriod()*1e-9));
	CameraDelayCount = (int)(CameraDelay/((double)RT::System::getInstance()->getPeriod()*1e-9));
	CameraDuration = 1.0/FrameRate;
	CameraDurationCount = (int)(CameraDuration/((double)RT::System::getInstance()->getPeriod()*1e-9));
	OffFrameStartCount = (int)(OffFrameDelay/((double)RT::System::getInstance()->getPeriod()*1e-9));
	printf("CameraCtl Settings: durationCnt=%d, LampDelayCnt=%d, CameraDelayCnt=%d, CameraDurationCt=%d\n", durationCount, LampDelayCount, CameraDelayCount, CameraDurationCount);

	output(1) = 5;
}
