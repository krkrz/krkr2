object EditLabelAttribFrame: TEditLabelAttribFrame
  Left = 0
  Top = 0
  Width = 383
  Height = 30
  HorzScrollBar.Visible = False
  VertScrollBar.Visible = False
  TabOrder = 0
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 45
    Height = 12
    Caption = 'ƒ‰ƒxƒ‹–¼'
  end
  object LabelNameEdit: TEdit
    Left = 64
    Top = 4
    Width = 305
    Height = 20
    TabOrder = 0
    OnChange = LabelNameEditChange
    OnExit = LabelNameEditExit
    OnKeyDown = LabelNameEditKeyDown
  end
end
