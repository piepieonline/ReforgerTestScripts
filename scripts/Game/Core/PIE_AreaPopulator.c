class PIE_AreaPopulator : Managed
{
	float paramSpawnRadius = 100;
	int paramBuildingsToFill = 5;
	int paramPatrolsCount = 5;
	int paramVehicleCount = 2;
	bool paramVehicleJeep = true;
	bool paramVehicleAPC = true;
	bool paramVehiclePatrols = false;
	string paramSpawnUnitFaction = "USSR";
	
	int valueFoundBuildings = -1;
	
	protected static ref array<PIE_AreaPopulator> areaPopulators = new array<PIE_AreaPopulator>;
	SCR_MapDescriptorComponent selectedMapDesc;
	
	private ref array<IEntity> foundBuildings = {};
	
	static void OpenDialog(SCR_MapDescriptorComponent mapDesc)
	{
		PIE_AreaPopulator populator = new PIE_AreaPopulator();
		populator.selectedMapDesc = mapDesc;
		
		populator.UpdateValues();
		auto menu = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.PIE_LayoutPopulateArea);
		SCR_PIE_PopulateAreaUI.Cast(menu).SetAreaPopulator(populator);
	}
	
	void UpdateValues()
	{
		MapItem selectedMapItem = selectedMapDesc.Item();
		
		foundBuildings.Clear();
		FindBuildingsNearPoint(selectedMapItem.Entity().GetOrigin(), paramSpawnRadius);
		valueFoundBuildings = foundBuildings.Count();
		paramBuildingsToFill = Math.Max(Math.Min(paramBuildingsToFill, valueFoundBuildings - 1), 0);
	}
	
	void Run()
	{
		SCR_HintManagerComponent.GetInstance().ShowCustomHint("PIE_AreaPopulator.Run()", "Running with spawn radius ", 5.0);
		MapItem selectedMapItem = selectedMapDesc.Item();
			
		UpdateValues();
		
		for(int i = 0; i < paramBuildingsToFill; i++)
		{
			if(PIE_ZeusControlComponent.GetLocalInstance())
			{
				Print("PIE_ZeusControlComponent.Instance.SpawnGarrisonUnit", LogLevel.NORMAL);
				SCR_HintManagerComponent.GetInstance().ShowCustomHint("PIE_ZeusControlComponent.Instance.SpawnGarrisonUnit", "Success", 10.0);
				PIE_ZeusControlComponent.GetLocalInstance().SpawnGarrisonUnit(paramSpawnUnitFaction, paramSpawnRadius, selectedMapItem.Entity().GetOrigin(), foundBuildings.GetRandomElement().GetOrigin());
			}
			else
			{
				SCR_HintManagerComponent.GetInstance().ShowCustomHint("PIE_ZeusControlComponent.Instance.SpawnGarrisonUnit", "Failure", 10.0);
				Print("No global spawner", LogLevel.ERROR);
			}
		}
		
		for(int i = 0; i < paramPatrolsCount; i++)
		{
			if(PIE_ZeusControlComponent.GetLocalInstance())
				PIE_ZeusControlComponent.GetLocalInstance().SpawnPatrolUnit(paramSpawnUnitFaction, paramSpawnRadius, selectedMapItem.Entity().GetOrigin());
			else
				Print("No global spawner", LogLevel.ERROR);
		}
		
		for(int i = 0; i < paramVehicleCount; i++)
		{
			if(PIE_ZeusControlComponent.GetLocalInstance())
				PIE_ZeusControlComponent.GetLocalInstance().SpawnVehicle(paramSpawnUnitFaction, paramSpawnRadius, selectedMapItem.Entity().GetOrigin(), paramVehiclePatrols, paramVehicleJeep, paramVehicleAPC);
			else
				Print("No global spawner", LogLevel.ERROR);
		}
	}
	
	void FindBuildingsNearPoint(vector center, float radius)
	{
		BaseWorld world = GetGame().GetWorld();
		if (!world) return;

		// Build AABB around the point
		vector mins = center - Vector(radius, radius, radius);
		vector maxs = center + Vector(radius, radius, radius);

		// Query all entities in the AABB
		foundBuildings = {};
		world.QueryEntitiesByAABB(mins, maxs, AddBuildingCallback);

		foreach (IEntity ent : foundBuildings)
		{
			if (!ent) continue;

			// Test if this entity is a building
			// You can adjust this depending on your tagging system:
			vector pos = ent.GetOrigin();
			float dist = vector.Distance(center, pos);
			PrintFormat("Building %1 at distance %2 at %3", ent, dist, pos, LogLevel.NORMAL);
		}
	}

	bool AddBuildingCallback(IEntity entity)
	{
		EntityPrefabData prefabData = entity.GetPrefabData();
		if(!prefabData) 
			return true;
		
		/*	
		ResourceName prefabName = prefabData.GetPrefabName();
		if (!prefabName.Contains("House") && !prefabName.Contains("Industrial"))
			return true;
		*/
		
 		// Skip buildings with no doors
		ref array<Managed> doors = {};
		FindComponentsInAllChildren(SCR_DestructibleBuildingComponent, entity, false, -1, -1, doors);
		if (doors.Count() == 0)
			return true;
				
		foundBuildings.Insert(entity);
		return true; // continue searching
	}
}