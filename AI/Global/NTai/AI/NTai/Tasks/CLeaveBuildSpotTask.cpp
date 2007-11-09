#include "../Core/helper.h"
// Tasks
#include "../Tasks/CLeaveBuildSpotTask.h"

CLeaveBuildSpotTask::CLeaveBuildSpotTask(Global* GL, int unit, weak_ptr<CUnitTypeData> ud){
	valid=true;
	G = GL;
	this->unit=unit;
	utd = ud.lock();
}

void CLeaveBuildSpotTask::RecieveMessage(CMessage &message){
	if(message.GetType() == string("unitidle")){
		if(message.GetParameter(0) == unit){
			End();
			//CMessage message2(string("taskfinished"));
			//FireEventListener(message2);
		}
	}else if(message.GetType() == string("type?")){
		message.SetType(" leave buildspot task");
	}
}

bool CLeaveBuildSpotTask::Init(){
	if(utd->IsFactory()){
        if(!utd->IsMobile()){
            End();
            return false;
        }
    }
    //G->L.iprint("CLeaveBuildSpotTask::Init");

	// register this modules listeners
	//	G->RegisterMessageHandler("unitidle",me);

	float3 pos = G->GetUnitPos(unit);

	if(!G->Map->CheckFloat3(pos)){
		End();
		return false;
	}

	bool rvalue=false;
	int facing = -1;

	int* a = new int[100];

	int n = G->cb->GetFriendlyUnits(a,pos,10);

	if(n > 1){
		for(int i = 0; i <n; i++){
			if( a[i] == unit) continue;
			shared_ptr<CUnitTypeData> ud2 = G->UnitDefLoader->GetUnitTypeDataByUnitId(a[i]).lock();
			if(ud2->IsFactory()){
				facing = G->cb->GetBuildingFacing(a[i]);
				break;
			}
		}
	}
	delete[] a;

	float3 diff;
	diff.z = 110;
	if(facing != -1){
		// 0 == facing south

		if(facing == 1){
			// 1 == facing west
			diff.z=0;
			diff.x = -140;
		}else if (facing == 2){
			// 2 == facing north
			diff.z -= 280;
		}else if (facing == 3){
			// 3 == facing east
			diff.z = 0;
			diff.x = 140;
		}
	}

	rvalue = G->Actions->Move(unit,pos+diff);

	if (!rvalue){
		End();
	}

	if(utd->IsFactory()){
	    End();
	}
	return rvalue;
}

void CLeaveBuildSpotTask::End(){
	NLOG("CLeaveBuildSpotTask::End");
	valid = false;
}
