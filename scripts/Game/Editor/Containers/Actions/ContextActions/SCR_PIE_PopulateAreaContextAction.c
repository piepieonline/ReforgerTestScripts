[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_Info")]
class SCR_PIE_PopulateArea : SCR_BaseContextAction
{
	ref set<SCR_EditableEntityComponent> m_LOL = new set<SCR_EditableEntityComponent>;
	ref array<Managed> mapDescriptors = {};
	
	override bool CanBeShown(SCR_EditableEntityComponent hoveredEntity, notnull set<SCR_EditableEntityComponent> selectedEntities, vector cursorWorldPosition, int flags)
	{
		ChimeraWorld world = GetGame().GetWorld();
		if (world.IsGameTimePaused())
			return false;
		
		//
		vector positionCheck;
		return !hoveredEntity || hoveredEntity.GetPos(positionCheck);
	}
	
	override bool CanBePerformed(SCR_EditableEntityComponent hoveredEntity, notnull set<SCR_EditableEntityComponent> selectedEntities, vector cursorWorldPosition, int flags)
	{
		return CanBeShown(hoveredEntity, selectedEntities, cursorWorldPosition, flags);
	}
	
	override void Perform(SCR_EditableEntityComponent hoveredEntity, notnull set<SCR_EditableEntityComponent> selectedEntities, vector cursorWorldPosition, int flags, int param = -1)
	{
		vector workingPosition = cursorWorldPosition;
		
		if (hoveredEntity)
			hoveredEntity.GetPos(workingPosition);
		
		PrintFormat("PIE_Modules_PopulateArea.Perform() method reached: %1", workingPosition, LogLevel.NORMAL);
		// SCR_HintManagerComponent.GetInstance().ShowCustomHint("PIE_Modules_PopulateArea.Perform()", "TEST GROUND", 3.0);
		
		if(m_LOL.IsEmpty())
		{
			SCR_EditableEntityCore core = SCR_EditableEntityCore.Cast(SCR_EditableEntityCore.GetInstance(SCR_EditableEntityCore));
			core.GetAllEntities(m_LOL);
			
			foreach(SCR_EditableEntityComponent comp : m_LOL)
			{
				FindComponentsInAllChildren(SCR_MapDescriptorComponent, comp.GetOwner(), false, -1, -1, mapDescriptors);
			}
			
			PrintFormat("PIE_Modules_PopulateArea: Entities found %1, descriptors found: %2", m_LOL.Count(), mapDescriptors.Count(), LogLevel.NORMAL);
		}
		else
		{
			Print("Using cached descriptors");
		}

		float distance = float.INFINITY;
		SCR_MapDescriptorComponent selectedMapDesc;
		
		foreach(auto desc : mapDescriptors)
		{
			SCR_MapDescriptorComponent mapDesc = SCR_MapDescriptorComponent.Cast(desc);
			if (!mapDesc) continue;

			MapItem selectedMapItem = mapDesc.Item();
			if (!selectedMapItem) continue;

			float sqDistance = vector.DistanceSq(workingPosition, selectedMapItem.Entity().GetOrigin());
			// PrintFormat("PIE_Modules_PopulateArea: %4 Distance %1, working pos: %2, selected pos: %3", sqDistance, workingPosition, selectedMapItem.Entity().GetOrigin(), mapDesc.Item().GetDisplayName(), LogLevel.NORMAL);
			if(sqDistance < distance)
			{
				distance = sqDistance;
				selectedMapDesc = mapDesc;
			}
		}
		
		if(selectedMapDesc != null)
		{
			PrintFormat("PIE_Modules_PopulateArea: Descriptor %1", selectedMapDesc.Item().GetDisplayName(), LogLevel.NORMAL);
			PrintFormat("PIE_Modules_PopulateArea: Type %1", SCR_Enum.GetEnumName(EMapDescriptorType, selectedMapDesc.GetBaseType()), LogLevel.NORMAL);

			Print("Spawning");
			
			PIE_AreaPopulator.OpenDialog(selectedMapDesc);
		}
	}
}