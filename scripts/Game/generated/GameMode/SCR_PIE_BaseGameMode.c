modded class SCR_BaseGameMode
{
	override void OnPlayerSpawned(int playerId, IEntity controlledEntity)
	{
		super.OnPlayerSpawned(playerId, controlledEntity);
		
		UUID playerUUID = SCR_PlayerIdentityUtils.GetPlayerIdentityId(playerId);
		
		string prefabName = controlledEntity.GetPrefabData().GetPrefabName();
		
		// PrintFormat("[MyMod] Player %1 spawned with prefab %2.", playerUUID, prefabName);
	
		// if(playerUUID == "a454168d-f4cf-4373-80c9-e522814d30aa") // Pie
		if(playerUUID == "a904bc0a-828c-4e8c-b583-ec93cd7d0c37" || prefabName.Contains("Medic")) // Damaged
		{
	        Resource prefab = Resource.Load("{99CFE4D74636ECB5}Prefabs/Characters/Armbands/Single/Armband_Medic.et");
	        if (!prefab) return;
	
			vector transform[4];
			controlledEntity.GetWorldTransform(transform);
			
			EntitySpawnParams params = new EntitySpawnParams();
			params.TransformMode = ETransformMode.WORLD;
			params.Transform = transform;
	        IEntity item = GetGame().SpawnEntityPrefab(prefab, GetGame().GetWorld(), params);
			
	        SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(controlledEntity.FindComponent(SCR_InventoryStorageManagerComponent));
	        inv.TryInsertItem(item);
		}
	}
}