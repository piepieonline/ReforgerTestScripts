class SCR_PIE_PopulateAreaUI : MenuBase
{
	protected static const string BUTTON_CLOSE = "ButtonClose";
	protected static const string BUTTON_CHANGE = "ButtonStart";

	ref PIE_AreaPopulator areaPopulator;

	private ref Widget rootWidget;
	private bool inUIUpdate = false;

	//------------------------------------------------------------------------------------------------
	protected override void OnMenuOpen()
	{
		rootWidget = GetRootWidget();
		if (!rootWidget)
		{
			Print("Error in Layout Tutorial layout creation", LogLevel.ERROR);
			return;
		}

		// Close button
		SCR_ButtonTextComponent buttonClose = SCR_ButtonTextComponent.GetButtonText(BUTTON_CLOSE, rootWidget);
		if (buttonClose)
			buttonClose.m_OnClicked.Insert(Close);
		else
			Print("Button Close not found - won't be able to exit by button", LogLevel.WARNING);

		// Change button
		SCR_ButtonTextComponent buttonChange = SCR_ButtonTextComponent.GetButtonText(BUTTON_CHANGE, rootWidget);
		if (buttonChange)
			buttonChange.m_OnClicked.Insert(RunSpawners);
		else
			Print("Button Change not found", LogLevel.WARNING);

		// ESC listeners
		InputManager inputManager = GetGame().GetInputManager();
		if (inputManager)
		{
			inputManager.AddActionListener("MenuOpen", EActionTrigger.DOWN, Close);
			inputManager.AddActionListener("MenuBack", EActionTrigger.DOWN, Close);
#ifdef WORKBENCH
			inputManager.AddActionListener("MenuOpenWB", EActionTrigger.DOWN, Close);
			inputManager.AddActionListener("MenuBackWB", EActionTrigger.DOWN, Close);
#endif
		}
		else if (!buttonClose)
		{
			Print("Auto-closing the menu that has no exit path", LogLevel.WARNING);
			Close();
			return;
		}

		SCR_ButtonTextComponent.GetButtonText("ButtonUS", rootWidget, true).m_OnClicked.Insert(SetFactionUS);
		SCR_ButtonTextComponent.GetButtonText("ButtonUSSR", rootWidget, true).m_OnClicked.Insert(SetFactionUSSR);
		SCR_ButtonTextComponent.GetButtonText("ButtonFIA", rootWidget, true).m_OnClicked.Insert(SetFactionFIA);
		
		// Widget listeners
		AddEventListeners("AreaSizeEdit");
		AddEventListeners("AreaSizeSlider");
		AddEventListeners("GarrisonEdit");
		AddEventListeners("GarrisonSlider");
		AddEventListeners("PatrolEdit");
		AddEventListeners("VehicleCountEdit");
		AddEventListeners("VehicleJeepsCheck");
		AddEventListeners("VehicleAPCsCheck");
	}

	protected override void OnMenuClose()
	{
		InputManager inputManager = GetGame().GetInputManager();
		if (inputManager)
		{
			inputManager.RemoveActionListener("MenuOpen", EActionTrigger.DOWN, Close);
			inputManager.RemoveActionListener("MenuBack", EActionTrigger.DOWN, Close);
#ifdef WORKBENCH
			inputManager.RemoveActionListener("MenuOpenWB", EActionTrigger.DOWN, Close);
			inputManager.RemoveActionListener("MenuBackWB", EActionTrigger.DOWN, Close);
#endif
		}
	}

	protected override void OnMenuUpdate(float tDelta)
	{
		if(areaPopulator)
		{
			ShapeFlags shapeFlags = ShapeFlags.ONCE | ShapeFlags.NOZBUFFER | ShapeFlags.TRANSP | ShapeFlags.DOUBLESIDE | ShapeFlags.NOOUTLINE;
			vector trans[4];
			areaPopulator.selectedMapDesc.Item().Entity().GetTransform(trans);
			SCR_Shape.DrawCircle(trans, areaPopulator.paramSpawnRadius, ARGB(50, 200, 200, 200), ARGB(50, 200, 200, 200), shapeFlags);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void RunSpawners()
	{
		areaPopulator.Run();
		Close();
	}

	void SetAreaPopulator(PIE_AreaPopulator populator)
	{
		areaPopulator = populator;
		UpdateUI();
	}

	//------------------------------------------------------------------------------------------------
	private void SetFactionUS()
	{
		areaPopulator.paramSpawnUnitFaction = "US";
		UpdateUI();
	}
	
	private void SetFactionUSSR()
	{
		areaPopulator.paramSpawnUnitFaction = "USSR";
		UpdateUI();
	}
	
	private void SetFactionFIA()
	{
		areaPopulator.paramSpawnUnitFaction = "FIA";
		UpdateUI();
	}
	
	private void AddEventListeners(string path)
	{
		Widget w = rootWidget.FindAnyWidget(path);
		if (!w)
			return;

		SCR_EventHandlerComponent handler = new SCR_EventHandlerComponent();
		handler.GetOnChange().Insert(WidgetOnChange);
		handler.GetOnChangeFinal().Insert(WidgetOnChangeFinal);

		w.AddHandler(handler);
	}

	//------------------------------------------------------------------------------------------------
	void UpdateUI(string ignoreElement = "")
	{
		inUIUpdate = true;

		UpdateTextElement("TitleText",
			string.Format("Populate Area - %1 - %2",
			areaPopulator.paramSpawnUnitFaction,
			areaPopulator.selectedMapDesc.Item().GetDisplayName()));

		if (ignoreElement != "AreaSizeEdit")
			UpdateEditTextElement("AreaSizeEdit", areaPopulator.paramSpawnRadius.ToString());

		if (ignoreElement != "AreaSizeSlider")
			UpdateSliderElement("AreaSizeSlider", areaPopulator.paramSpawnRadius, 0, 1000);

		if (ignoreElement != "GarrisonEdit")
			UpdateEditTextElement("GarrisonEdit", areaPopulator.paramBuildingsToFill.ToString());

		if (ignoreElement != "GarrisonSlider")
			UpdateSliderElement("GarrisonSlider", areaPopulator.paramBuildingsToFill, 0, areaPopulator.valueFoundBuildings);

		if (ignoreElement != "PatrolEdit")
			UpdateEditTextElement("PatrolEdit", areaPopulator.paramPatrolsCount.ToString());
		
		if (ignoreElement != "VehicleCountEdit")
			UpdateEditTextElement("VehicleCountEdit", areaPopulator.paramVehicleCount.ToString());
		
		if (ignoreElement != "VehicleJeepsCheck")
			UpdateCheckElement("VehicleJeepsCheck", areaPopulator.paramVehicleJeep);
		
		if (ignoreElement != "VehicleAPCsCheck")
			UpdateCheckElement("VehicleAPCsCheck", areaPopulator.paramVehicleAPC);
		
		UpdateTextElement("GarrisonMaxNumber", areaPopulator.valueFoundBuildings.ToString());

		inUIUpdate = false;
	}

	//------------------------------------------------------------------------------------------------
	private void UpdateTextElement(string path, string value)
	{
		TextWidget w = TextWidget.Cast(rootWidget.FindAnyWidget(path));
		if (w)
			w.SetText(value);
	}

	private void UpdateEditTextElement(string path, string value)
	{
		EditBoxWidget w = EditBoxWidget.Cast(rootWidget.FindAnyWidget(path));
		if (w)
			w.SetText(value);
	}

	private void UpdateSliderElement(string path, int value, int min = -1, int max = -1)
	{
		SliderWidget w = SliderWidget.Cast(rootWidget.FindAnyWidget(path));
		if (!w)
			return;

		if (min >= 0 && max >= 0 && min <= max)
		{
			w.SetMin(min);
			w.SetMax(max);
		}

		w.SetCurrent(value);
	}
	
	private void UpdateCheckElement(string path, bool value)
	{
		CheckBoxWidget w = CheckBoxWidget.Cast(rootWidget.FindAnyWidget(path));
		if(w)
			w.SetChecked(value);
	}

	//------------------------------------------------------------------------------------------------
	protected void WidgetOnChange(Widget w)
	{
		if (inUIUpdate)
			return;

		UpdateValueFromWidget(w);

		inUIUpdate = true;
		UpdateUI(w.GetName());
		inUIUpdate = false;
	}

	protected void WidgetOnChangeFinal(Widget w)
	{
		if (inUIUpdate)
			return;
		inUIUpdate = true;

		UpdateValueFromWidget(w);
		areaPopulator.UpdateValues();
		UpdateUI(w.GetName());

		inUIUpdate = false;
	}
	
	protected void UpdateValueFromWidget(Widget w)
	{
		switch (w.GetName())
		{
			case "AreaSizeSlider":
			{
				areaPopulator.paramSpawnRadius = SliderWidget.Cast(w).GetCurrent();
				break;
			}
			case "AreaSizeEdit":
			{
				areaPopulator.paramSpawnRadius = EditBoxWidget.Cast(w).GetText().ToInt();
				break;
			}
			case "GarrisonSlider":
			{
				areaPopulator.paramBuildingsToFill = SliderWidget.Cast(w).GetCurrent();
				break;
			}
			case "GarrisonEdit":
			{
				areaPopulator.paramBuildingsToFill = EditBoxWidget.Cast(w).GetText().ToInt();
				break;
			}
			case "PatrolEdit":
			{
				areaPopulator.paramPatrolsCount = EditBoxWidget.Cast(w).GetText().ToInt();
				break;
			}
			case "VehicleCountEdit":
			{
				areaPopulator.paramVehicleCount = EditBoxWidget.Cast(w).GetText().ToInt();
				break;
			}
			case "VehicleJeepsCheck":
			{
				areaPopulator.paramVehicleJeep = CheckBoxWidget.Cast(w).IsChecked();
				break;
			}
			case "VehicleAPCsCheck":
			{
				areaPopulator.paramVehicleAPC = CheckBoxWidget.Cast(w).IsChecked();
				break;
			}
		}
	}	
}
