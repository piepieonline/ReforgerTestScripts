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
			Print("Button Change not found", LogLevel.WARNING); // the button can be missing without putting the layout in jeopardy

		/*
			ESC/Start listener
		*/

		InputManager inputManager = GetGame().GetInputManager();
		if (inputManager)
		{
			// this is for the menu/dialog to close when pressing ESC
			// an alternative is to have a button with the SCR_NavigationButtonComponent component
			// and its Action Name field set to MenuBack - this would activate the button on ESC press
			inputManager.AddActionListener("MenuOpen", EActionTrigger.DOWN, Close);
			inputManager.AddActionListener("MenuBack", EActionTrigger.DOWN, Close);
#ifdef WORKBENCH // in Workbench, F10 is used because ESC closes the preview
			inputManager.AddActionListener("MenuOpenWB", EActionTrigger.DOWN, Close);
			inputManager.AddActionListener("MenuBackWB", EActionTrigger.DOWN, Close);
#endif // WORKBENCH
		}
		else if (!buttonClose)
		{
			Print("Auto-closing the menu that has no exit path", LogLevel.WARNING);
			Close();
			return;
		}
		
		// Widget listeners
		AddEventListeners("AreaSizeEdit");
		AddEventListeners("AreaSizeSlider");
		AddEventListeners("GarrisonEdit");
		AddEventListeners("GarrisonSlider");
	}

	//------------------------------------------------------------------------------------------------
	protected override void OnMenuClose()
	{
		// here we clean action listeners added above as the good practice wants it
		InputManager inputManager = GetGame().GetInputManager();
		if (inputManager)
		{
			inputManager.RemoveActionListener("MenuOpen", EActionTrigger.DOWN, Close);
			inputManager.RemoveActionListener("MenuBack", EActionTrigger.DOWN, Close);
#ifdef WORKBENCH
			inputManager.RemoveActionListener("MenuOpenWB", EActionTrigger.DOWN, Close);
			inputManager.RemoveActionListener("MenuBackWB", EActionTrigger.DOWN, Close);
#endif // WORKBENCH
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
	
	private void AddEventListeners(string path)
	{
		SCR_EventHandlerComponent eventHandler = new SCR_EventHandlerComponent();
		eventHandler.GetOnChange().Insert(WidgetOnChange);
		eventHandler.GetOnChangeFinal().Insert(WidgetOnChangeFinal);
		rootWidget.FindAnyWidget(path).AddHandler(eventHandler);
	}
	
	void UpdateUI(string ignoreElement = "")
	{
		UpdateTextElement("TitleText", string.Format("Populate Area - %1", areaPopulator.selectedMapDesc.Item().GetDisplayName()));
		
		if(ignoreElement != "AreaSizeEdit")
			UpdateEditTextElement("AreaSizeEdit", string.Format("%1", areaPopulator.paramSpawnRadius));
		if(ignoreElement != "AreaSizeSlider")
			UpdateSliderElement("AreaSizeSlider", areaPopulator.paramSpawnRadius, 0, 1000);
		
		if(ignoreElement != "GarrisonEdit")
			UpdateEditTextElement("GarrisonEdit", string.Format("%1", areaPopulator.paramBuildingsToFill));
		if(ignoreElement != "GarrisonSlider")
			UpdateSliderElement("GarrisonSlider", areaPopulator.paramBuildingsToFill, 0, areaPopulator.valueFoundBuildings);
		UpdateTextElement("GarrisonMaxNumber", string.Format("%1", areaPopulator.valueFoundBuildings));
	}
	
	private void UpdateTextElement(string path, string value)
	{
		if (!rootWidget)
			return;
		
		TextWidget textWidget = TextWidget.Cast(rootWidget.FindAnyWidget(path));
		if (!textWidget)
		{
			PrintFormat("%1 as TextWidget could not be found", path, LogLevel.WARNING);
			return;
		}

		textWidget.SetText(value);
	}
	
	private void UpdateEditTextElement(string path, string value)
	{
		if (!rootWidget)
			return;
		
		EditBoxWidget textWidget = EditBoxWidget.Cast(rootWidget.FindAnyWidget(path));
		if (!textWidget)
		{
			PrintFormat("%1 as EditBoxWidget could not be found", path, LogLevel.WARNING);
			return;
		}

		textWidget.SetText(value);
	}
	
	private void UpdateSliderElement(string path, int value, int min = -1, int max = -1)
	{
		if (!rootWidget)
			return;
		
		SliderWidget widget = SliderWidget.Cast(rootWidget.FindAnyWidget(path));
		if (!widget)
		{
			PrintFormat("%1 as SliderWidget could not be found", path, LogLevel.WARNING);
			return;
		}
		
		if(min >=0 && max >= 0 && min <= max)
		{
			widget.SetMin(min);
			widget.SetMax(max);
		}

		widget.SetCurrent(value);
	}
	
	protected void WidgetOnChange(Widget w)
	{
		if(inUIUpdate)
		{
			return;
		}
		
		inUIUpdate = true;
		
		switch(w.GetName())
		{
			case "AreaSizeSlider":
				areaPopulator.paramSpawnRadius = SliderWidget.Cast(w).GetCurrent();
				break;
			case "AreaSizeEdit":
				areaPopulator.paramSpawnRadius = EditBoxWidget.Cast(w).GetText().ToInt();
				break;
			case "GarrisonSlider":
				areaPopulator.paramBuildingsToFill = SliderWidget.Cast(w).GetCurrent();
				break;
			case "GarrisonEdit":
				areaPopulator.paramBuildingsToFill = EditBoxWidget.Cast(w).GetText().ToInt();
				break;
		}
		
		UpdateUI(w.GetName());
		
		inUIUpdate = false;
	}
	
	protected void WidgetOnChangeFinal(Widget w)
	{
		inUIUpdate = true;
		WidgetOnChange(w);
		inUIUpdate = false;
		areaPopulator.UpdateValues();
		UpdateUI(w.GetName());
	}
}