class PIE_AreaPopulator : Managed
{
	float paramSpawnRadius = 100;
	int paramBuildingsToFill = 5;
	
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
		paramBuildingsToFill = Math.Min(paramBuildingsToFill, valueFoundBuildings - 1);
	}
	
	void Run()
	{
		MapItem selectedMapItem = selectedMapDesc.Item();
			
		UpdateValues();

		foreach (IEntity ent : foundBuildings)
		{
			SpawnUnit(selectedMapItem.Entity().GetOrigin(), ent);
		}
	}
	
	void SpawnUnit(vector spawnPos, IEntity ent)
	{
		vector position = ent.GetOrigin();
		Resource groupPrefab = Resource.Load("{5BEA04939D148B1D}Prefabs/Groups/INDFOR/Group_FIA_FireTeam.et");
		// Resource groupPrefab = Resource.Load("{84B40583F4D1B7A3}Prefabs/Characters/Factions/INDFOR/FIA/Character_FIA_Rifleman.et");
		IEntity spawnedGroupEntity = GetGame().SpawnEntityPrefab(groupPrefab, GetGame().GetWorld(), GenerateSpawnParameters(spawnPos));

		SCR_AIGroup group = SCR_AIGroup.Cast(spawnedGroupEntity);

		if(group != null)
		{
			//Resource waypointResource = Resource.Load("{750A8D1695BD6998}Prefabs/AI/Waypoints/AIWaypoint_Move.et");
			Resource waypointResource = Resource.Load("{93291E72AC23930F}Prefabs/AI/Waypoints/AIWaypoint_Defend.et");
			vector waypointPos = position;
			AIWaypoint waypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(waypointResource, null, GenerateSpawnParameters(waypointPos)));
			waypoint.SetCompletionRadius(5.0);
			group.AddWaypoint(waypoint);

			PrintFormat("PIE_Modules_PopulateArea: Spawned at %1 (%4) with waypoint %2 at %3", position, waypoint, waypointPos, ent.GetPrefabData().GetPrefabName(), LogLevel.NORMAL);
		}
		else
		{
			PrintFormat("PIE_Modules_PopulateArea: Spawn at %1 (%2) without a group", position, ent.GetPrefabData().GetPrefabName(), LogLevel.ERROR);
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

	protected EntitySpawnParams GenerateSpawnParameters(vector position)
	{
		// Create a new set of spawn parameters 
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		
		// Assign the position to those parameters 
		params.Transform[3] = position;
		
		// Return this set of spawn parameters
		return params;
	}
}