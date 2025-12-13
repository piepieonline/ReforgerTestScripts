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
	
	static ref map<string, ref array<string>> unitTypes = new map<string, ref array<string>>();
	static ref map<string, ref array<string>> jeepTypes = new map<string, ref array<string>>();
	static ref map<string, ref array<string>> apcTypes = new map<string, ref array<string>>();
	
	protected static ref array<PIE_AreaPopulator> areaPopulators = new array<PIE_AreaPopulator>;
	SCR_MapDescriptorComponent selectedMapDesc;
	
	private ref array<IEntity> foundBuildings = {};
	
	static void OpenDialog(SCR_MapDescriptorComponent mapDesc)
	{
		PIE_AreaPopulator populator = new PIE_AreaPopulator();
		populator.selectedMapDesc = mapDesc;
		
		populator.unitTypes.Clear();
		array<string> usUnits = { "{84E5BBAB25EA23E5}Prefabs/Groups/BLUFOR/Group_US_FireTeam.et", "{DDF3799FA1387848}Prefabs/Groups/BLUFOR/Group_US_RifleSquad.et" };
		array<string> ussrUnits = { "{30ED11AA4F0D41E5}Prefabs/Groups/OPFOR/Group_USSR_FireGroup.et", "{E552DABF3636C2AD}Prefabs/Groups/OPFOR/Group_USSR_RifleSquad.et" };
		array<string> fiaUnits = { "{5BEA04939D148B1D}Prefabs/Groups/INDFOR/Group_FIA_FireTeam.et", "{CE41AF625D05D0F0}Prefabs/Groups/INDFOR/Group_FIA_RifleSquad.et" };
		unitTypes.Set("US", usUnits);
		unitTypes.Set("USSR", ussrUnits);
		unitTypes.Set("FIA", fiaUnits);
		
		populator.jeepTypes.Clear();
		array<string> usJeeps = { "{3EA6F47D95867114}Prefabs/Vehicles/Wheeled/M998/M1025_armed_M2HB.et", "{26A9756790131354}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Rifleman.et", "{26A9756790131354}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Rifleman.et" };
		array<string> ussrJeeps = { "{254289B9C09904AB}Prefabs/Vehicles/Wheeled/BRDM2/BRDM2.et", "{9FFEF10757E742EB}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_Crew.et", "{9FFEF10757E742EB}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_Crew.et", "{9FFEF10757E742EB}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_Crew.et"};
		array<string> fiaJeeps = { "{0B4DEA8078B78A9B}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_PKM.et", "{84B40583F4D1B7A3}Prefabs/Characters/Factions/INDFOR/FIA/Character_FIA_Rifleman.et", "{84B40583F4D1B7A3}Prefabs/Characters/Factions/INDFOR/FIA/Character_FIA_Rifleman.et" };
		jeepTypes.Set("US", usJeeps);
		jeepTypes.Set("USSR", ussrJeeps);
		jeepTypes.Set("FIA", fiaJeeps);
		
		populator.apcTypes.Clear();
		array<string> usAPCs = { "{0FBF8F010F81A4E5}Prefabs/Vehicles/Wheeled/LAV25/LAV25.et", "{E1CB513B8B9B08F4}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Crew.et", "{E1CB513B8B9B08F4}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Crew.et", "{E1CB513B8B9B08F4}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Crew.et" };
		array<string> ussrAPCs = { "{C012BB3488BEA0C2}Prefabs/Vehicles/Wheeled/BTR70/BTR70.et", "{9FFEF10757E742EB}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_Crew.et", "{9FFEF10757E742EB}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_Crew.et", "{9FFEF10757E742EB}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_Crew.et" };
		array<string> fiaAPCs = { "{442939C9617DF228}Prefabs/Vehicles/Wheeled/BRDM2/BRDM2_FIA.et", "{641AD7731E23454B}Prefabs/Characters/Factions/INDFOR/FIA/Character_FIA_Crew.et", "{641AD7731E23454B}Prefabs/Characters/Factions/INDFOR/FIA/Character_FIA_Crew.et" };
		apcTypes.Set("US", usAPCs);
		apcTypes.Set("USSR", ussrAPCs);
		apcTypes.Set("FIA", fiaAPCs);
		
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
		MapItem selectedMapItem = selectedMapDesc.Item();
			
		UpdateValues();
		
		for(int i = 0; i < paramBuildingsToFill; i++)
		{
			SpawnGarrisonUnit(selectedMapItem.Entity().GetOrigin(), foundBuildings.GetRandomElement());
		}
		
		for(int i = 0; i < paramPatrolsCount; i++)
		{
			SpawnPatrolUnit(selectedMapItem.Entity().GetOrigin());
		}
		
		for(int i = 0; i < paramVehicleCount; i++)
		{
			SpawnVehicle(selectedMapItem.Entity().GetOrigin(), false);
		}
	}
	
	void SpawnGarrisonUnit(vector townPos, IEntity ent)
	{
		vector position = ent.GetOrigin();
		vector spawnPosition;
		SCR_WorldTools.FindEmptyTerrainPosition(spawnPosition, position, 150);
		Resource groupPrefab = Resource.Load(unitTypes.Get(paramSpawnUnitFaction).GetRandomElement());
		IEntity spawnedGroupEntity = GetGame().SpawnEntityPrefab(groupPrefab, GetGame().GetWorld(), GenerateSpawnParameters(spawnPosition));

		SCR_AIGroup group = SCR_AIGroup.Cast(spawnedGroupEntity);

		if(group != null)
		{
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

	void SpawnPatrolUnit(vector townPos)
	{
		vector cycleWaypointPos = GetRandomOffsetPosition(townPos, 150);
		Resource groupPrefab = Resource.Load(unitTypes.Get(paramSpawnUnitFaction).GetRandomElement());
		IEntity spawnedGroupEntity = GetGame().SpawnEntityPrefab(groupPrefab, GetGame().GetWorld(), GenerateSpawnParameters(cycleWaypointPos));
		
		Resource moveWaypointResource = Resource.Load("{750A8D1695BD6998}Prefabs/AI/Waypoints/AIWaypoint_Move.et");
		Resource cycleWaypointResource = Resource.Load("{35BD6541CBB8AC08}Prefabs/AI/Waypoints/AIWaypoint_Cycle.et");
		SCR_AIGroup group = SCR_AIGroup.Cast(spawnedGroupEntity);
		
		AIWaypoint moveWaypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(moveWaypointResource, null, GenerateSpawnParameters(GetRandomOffsetPosition(townPos, 150))));
		moveWaypoint.SetCompletionRadius(5.0);
		
		AIWaypoint moveCycleWaypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(moveWaypointResource, null, GenerateSpawnParameters(cycleWaypointPos)));
		moveCycleWaypoint.SetCompletionRadius(5.0);
		
		AIWaypointCycle cycleWaypoint = AIWaypointCycle.Cast(GetGame().SpawnEntityPrefab(cycleWaypointResource, null, GenerateSpawnParameters(cycleWaypointPos)));
		cycleWaypoint.SetWaypoints({moveWaypoint, moveCycleWaypoint});
		cycleWaypoint.SetRerunCounter(10000);
		group.AddWaypoint(cycleWaypoint);
	}
	
	void SpawnVehicle(vector townPos, bool patrol)
	{
		array<string> spawnTypes = new array<string>();
		if(paramVehicleJeep) 
			spawnTypes.Insert("jeep");
		
		if(paramVehicleAPC)
			spawnTypes.Insert("apc");
		
		if(spawnTypes.Count() == 0)
			return;
		
		string vicType = spawnTypes.GetRandomElement();
		string vicResourcePath;
		array<string> crewResourcePaths = new array<string>();
		
		if(vicType == "jeep")
		{
			vicResourcePath = jeepTypes.Get(paramSpawnUnitFaction)[0];
			for(int i = 1; i < jeepTypes.Get(paramSpawnUnitFaction).Count(); i++)
			{
				crewResourcePaths.Insert(jeepTypes.Get(paramSpawnUnitFaction)[i]);
			}
		}
		else if (vicType == "apc")
		{
			vicResourcePath = apcTypes.Get(paramSpawnUnitFaction)[0];
			for(int i = 1; i < apcTypes.Get(paramSpawnUnitFaction).Count(); i++)
			{
				crewResourcePaths.Insert(apcTypes.Get(paramSpawnUnitFaction)[i]);
			}
		}
		
		vector spawnPosition, spawnRotation;
		// TODO: On road
		GetRandomRoadSegmentTransform(townPos, 150, spawnPosition, spawnRotation);
		SCR_WorldTools.FindEmptyTerrainPosition(spawnPosition, spawnPosition, 150);
		
		IEntity groupEntity = GetGame().SpawnEntityPrefab(Resource.Load("{000CD338713F2B5A}Prefabs/AI/Groups/Group_Base.et"), GetGame().GetWorld(), GenerateSpawnParameters(spawnPosition));
		SCR_AIGroup aiGroup = SCR_AIGroup.Cast(groupEntity);
		aiGroup.m_faction = paramSpawnUnitFaction;
		
		IEntity spawnedVehicleEntity = GetGame().SpawnEntityPrefab(Resource.Load(vicResourcePath), GetGame().GetWorld(), GenerateSpawnParameters(spawnPosition));
		aiGroup.AddVehiclesStatic({ spawnedVehicleEntity.GetName() });
		
		foreach(string crew : crewResourcePaths)
		{
			IEntity spawnedCrewEntity = GetGame().SpawnEntityPrefab(Resource.Load(crew), GetGame().GetWorld(), GenerateSpawnParameters(spawnPosition));	
			aiGroup.AddAIEntityToGroup(spawnedCrewEntity);
		}
		
		AIWaypoint getInWaypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(Resource.Load("{B049D4C74FBC0C4D}Prefabs/AI/Waypoints/AIWaypoint_GetInNearest.et"), null, GenerateSpawnParameters(spawnPosition)));
		// SCR_AIEntityWaypointParameters getInParams = SCR_AIEntityWaypointParameters.Cast(getInWaypoint.FindComponent(SCR_AIEntityWaypointParameters));
		// getInParams.SetEntity(spawnedVehicleEntity);
		aiGroup.AddWaypoint(getInWaypoint);
	}
	
	static void GetRandomRoadSegmentTransform(
		vector centerPos,
		float offset,
		out vector outPos,
		out vector outDir
	)
	{
		SCR_AIWorld aiWorld = SCR_AIWorld.Cast(GetGame().GetAIWorld());
		if (!aiWorld)
		{
			return;
		}

		RoadNetworkManager roadNetworkManager = aiWorld.GetRoadNetworkManager();
		array<BaseRoad> roads = new array<BaseRoad>();
		roadNetworkManager.GetRoadsInAABB(
			Vector(centerPos[0] - offset / 2, centerPos[1], centerPos[2] - offset / 2),
			Vector(centerPos[0] + offset / 2, centerPos[1], centerPos[2] + offset / 2),
			roads
		);
		
		array<vector> roadPoints = new array<vector>();
		roads.GetRandomElement().GetPoints(roadPoints);
		
		outPos = roadPoints.GetRandomElement();
		outDir = vector.Zero;
	}

	static vector GetRandomOffsetPosition(vector center, float radius)
	{
		float angle = Math.RandomFloat(0.0, Math.PI2);
		float dist  = Math.Sqrt(Math.RandomFloat(0.0, 1.0)) * radius;
	
		float x = center[0] + Math.Cos(angle) * dist;
		float z = center[2] + Math.Sin(angle) * dist;

		vector outPos;
		SCR_WorldTools.FindEmptyTerrainPosition(outPos, Vector(x, center[1], z), 150);
		return outPos;
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