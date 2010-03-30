object ConfSettingsForm: TConfSettingsForm
  Left = 56
  Top = 447
  Width = 607
  Height = 406
  Caption = '設定'
  Color = clBtnFace
  Constraints.MinHeight = 389
  Constraints.MinWidth = 607
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'ＭＳ Ｐゴシック'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefaultPosOnly
  PixelsPerInch = 96
  TextHeight = 12
  object SpecifiedFileIsNotKrKrLabel: TLabel
    Left = 8
    Top = 328
    Width = 324
    Height = 12
    Caption = '指定されたファイルは吉里吉里実行可能ファイルではありません'
    Visible = False
  end
  object IconChangeWarnLabel: TLabel
    Left = 8
    Top = 337
    Width = 407
    Height = 12
    Anchors = [akLeft, akBottom]
    Caption = 
      '※ 一度アイコンの変更を行うと別のアイコンに変更できなくなる可能' +
      '性があります'
  end
  object FileWroteLabel: TLabel
    Left = 8
    Top = 336
    Width = 98
    Height = 12
    Caption = '設定を保存しました'
    Visible = False
  end
  object ProgramMayRunningLabel: TLabel
    Left = 8
    Top = 344
    Width = 322
    Height = 12
    Caption = 'ファイルを開けません(プログラムが実行中の可能性があります)'
    Visible = False
  end
  object OKButton: TButton
    Left = 425
    Top = 344
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'OK'
    Default = True
    TabOrder = 0
    OnClick = OKButtonClick
  end
  object CancelButton: TButton
    Left = 505
    Top = 344
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Cancel = True
    Caption = 'キャンセル'
    ModalResult = 2
    TabOrder = 1
  end
  inline ConfMainFrame: TConfMainFrame
    Left = 8
    Top = 8
    Width = 578
    Height = 323
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 2
    inherited IconGroupBox: TGroupBox
      Top = 260
      Width = 578
      TabOrder = 1
    end
    inherited OptionsGroupBox: TGroupBox
      Width = 578
      Height = 201
      inherited Label2: TLabel
        Left = 360
      end
      inherited OptionsTreeView: TTreeView
        TabOrder = 2
      end
      inherited OptionValueComboBox: TComboBox
        Left = 360
      end
      inherited OptionDescMemo: TMemo
        TabOrder = 0
      end
      inherited RestoreDefaultButton: TButton
        Width = 121
      end
    end
    inherited ReleaseOptionGroupBox: TGroupBox
      Top = 210
      Width = 578
      TabOrder = 2
    end
    inherited Panel1: TPanel
      Top = 201
      Width = 578
    end
    inherited Panel2: TPanel
      Top = 251
      Width = 578
    end
  end
  object OpenDialog: TOpenDialog
    DefaultExt = 'exe'
    Filter = '実行可能ファイル(*.exe)|*.exe|すべてのファイル(*.*)|*.*'
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofNoReadOnlyReturn, ofEnableSizing]
    Title = '吉里吉里 実行可能ファイルの選択'
    Left = 384
    Top = 328
  end
end
