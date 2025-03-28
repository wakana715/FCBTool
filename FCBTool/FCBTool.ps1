# .DESCRIPTION
# FCBToolのGUIスクリプト
# FCBTool.ps1 prm status progress
# .PARAMETER	Arg[0]		[Out]		パラメータファイル、GUIの入力値を出力する
# .PARAMETER	Arg[1]		[In,Out]	ステータスファイル、状態を入出力する
# .PARAMETER	Arg[2]		[In]		プログレスファイル、進捗を取得する
if ($Args.Count -lt 3)
{
    exit
}
Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing
$parameterFile    = $Args[0]
$statusFile       = $Args[1]
$progressFile     = $Args[2]
$form             = New-Object System.Windows.Forms.Form
$comboBoxSrc      = New-Object System.Windows.Forms.ComboBox
$buttonOpenSrc    = New-Object System.Windows.Forms.Button
$comboBoxDst      = New-Object System.Windows.Forms.ComboBox
$buttonOpenDst    = New-Object System.Windows.Forms.Button
$comboBoxFcb      = New-Object System.Windows.Forms.ComboBox
$buttonOpenFcb    = New-Object System.Windows.Forms.Button
$radioButtonLeft  = New-Object System.Windows.Forms.RadioButton
$radioButtonRight = New-Object System.Windows.Forms.RadioButton
$progressBarRun   = New-Object System.Windows.Forms.ProgressBar
$buttonRun        = New-Object System.Windows.Forms.Button
$buttonStop       = New-Object System.Windows.Forms.Button
$timer            = New-Object System.Windows.Forms.Timer
$aryComboBox      = $comboBoxSrc,   $comboBoxDst,   $comboBoxFcb
$aryButtonOpen    = $buttonOpenSrc, $buttonOpenDst, $buttonOpenFcb
$aryDialogTitle   = "Select Source File", "Select Dest File", "Select FCB File"
$aryName          =  "src",  "dst",  "fcb",  "opt"
$aryFind          = $False, $False, $False, $False
$aryText          =     "",     "",     "",     ""

# .DESCRIPTION
# ファイルダイアログでファイル名を入力する。
# 入力したファイル名を引数指定のコントロールへ設定する。
# .PARAMETER	senderFrom	未使用
# .PARAMETER	e			未使用
# .PARAMETER	index		ボタン配列(aryButtonOpen)、ダイアログタイトル配列(aryDialogTitle)、コンボボックス配列(aryComboBox)のインデックス
function buttonOpen_Click()
{
    param
    (
        [System.Object]$senderFrom,
        [System.EventArgs]$e,
        [int]$index
    )
    $aryButtonOpen[$index].Enabled = $False
    $form.Refresh()
    $dialog=New-Object System.Windows.Forms.OpenFileDialog
    $dialog.Title = $aryDialogTitle[$index]
    if ($dialog.ShowDialog() -eq [System.Windows.Forms.DialogResult]::OK)
    {
        $aryComboBox[$index].Text = $dialog.FileName
    }
    $aryButtonOpen[$index].Enabled = $True
    $form.Refresh()
}

# .DESCRIPTION
# srcのオープンボタンイベント関数。
# .PARAMETER	senderFrom	未使用
# .PARAMETER	e			未使用
function buttonOpenSrc_Click()
{
    param
    (
        [System.Object]$senderFrom,
        [System.EventArgs]$e
    )
    buttonOpen_Click $senderFrom $e 0
}

# .DESCRIPTION
# dstのオープンボタンイベント関数。
# .PARAMETER	senderFrom	未使用
# .PARAMETER	e			未使用
function buttonOpenDst_Click()
{
    param
    (
        [System.Object]$senderFrom,
        [System.EventArgs]$e
    )
    buttonOpen_Click $senderFrom $e 1
}

# .DESCRIPTION
# fcbのオープンボタンイベント関数。
# .PARAMETER	senderFrom	未使用
# .PARAMETER	e			未使用
function buttonOpenFcb_Click()
{
    param
    (
        [System.Object]$senderFrom,
        [System.EventArgs]$e
    )
    buttonOpen_Click $senderFrom $e 2
}

# .DESCRIPTION
# Stopボタンのイベント関数。
# .PARAMETER	senderFrom	未使用
# .PARAMETER	e			未使用
function buttonStop_Click()
{
    param
    (
        [System.Object]$senderFrom,
        [System.EventArgs]$e
    )
    Set-Content -Path $statusFile -Value "0003" -Encoding ascii # 3:中断要求
}

# .DESCRIPTION
# Runボタンのイベント関数。
# .PARAMETER	senderFrom	未使用
# .PARAMETER	e			未使用
function buttonRun_Click()
{
    param
    (
        [System.Object]$senderFrom,
        [System.EventArgs]$e
    )
    for ($i = 0; $i -lt $aryName.Count; $i++)
    {
        if ($aryName[$i] -eq "opt")
        {
            $aryText[$i] = ""
            if ($radioButtonLeft.Checked -eq $True)
            {
                $aryText[$i] = "1"
            }
            if ($radioButtonRight.Checked -eq $True)
            {
                $aryText[$i] = "2"
            }
            if ($aryText[$i] -eq "")
            {
                [System.Windows.Forms.MessageBox]::Show("Select opt", "Error", 0, "Error") # 0:[OK]
                return
            }
        }
        else
        {
            $aryText[$i] = $aryComboBox[$i].Text.Trim()
            if ($aryName[$i] -eq "dst")
            {
                if ($aryText[$i] -eq "")
                {
                    [System.Windows.Forms.MessageBox]::Show("input dst", "Error", 0, "Error") # 0:[OK]
                    return
                }
            }
            else
            {   #src or fcb
                if ($aryText[$i] -eq "" -Or (Test-Path $aryText[$i]) -eq $False)
                {
                    [System.Windows.Forms.MessageBox]::Show($aryText[$i] + " is not found", "Error", 0, "Error") # 0:[OK]
                    return
                }
            }
        }
    }
    $res = [System.Windows.Forms.MessageBox]::Show("Do you want to run it?", "Information", 1, "Information") # 1:[OK][Cancel]
    if ($res -eq "OK")
    {
        # 画面の入力内容をパラメータファイル(UTF-16)に保存
        Set-Content -Path $parameterFile -Value "K=V"  -Encoding unicode # DUMMY
        for ($i = 0; $i -lt $aryName.Count; $i++)
        {
            $value = $aryName[$i] + "=" + $aryText[$i]
            Add-Content -Path $parameterFile -Value $value -Encoding unicode
        }
        Set-Content -Path $statusFile -Value "0001" -Encoding ascii # 1:実行要求

        $comboBoxSrc.Enabled = $False
        $comboBoxDst.Enabled = $False
        $comboBoxFcb.Enabled = $False
        $buttonOpenSrc.Enabled = $False
        $buttonOpenDst.Enabled = $False
        $buttonOpenFcb.Enabled = $False
        $buttonStop.Enabled = $True
        $buttonRun.Enabled  = $False
        $progressBarRun.Maximum = 0
        $progressBarRun.Value   = 0
        $form.Refresh()
        $timer.Enabled = $True
    }
}

# .DESCRIPTION
# タイマイベント関数。
# .PARAMETER	senderFrom	未使用
# .PARAMETER	e			未使用
function timer_Tick()
{
    param
    (
        [System.Object]$senderFrom,
        [System.EventArgs]$e
    )
    $strStatus = Get-Content -Path $statusFile -Encoding ascii
    $status = [Convert]::ToInt64($strStatus, 16)
    if ($status -eq 0)      # 0:NOP
    {
        $timer.Enabled = $False
        $comboBoxSrc.Enabled = $True
        $comboBoxDst.Enabled = $True
        $comboBoxFcb.Enabled = $True
        $buttonOpenSrc.Enabled = $True
        $buttonOpenDst.Enabled = $True
        $buttonOpenFcb.Enabled = $True
        $buttonStop.Enabled = $False
        $buttonRun.Enabled  = $True
        $progressBarRun.Value = $progressBarRun.Maximum
        $form.Refresh()
    }
    elseif ($status -eq 2)  # 2:実行中
    {
        $progress = Get-Content -Path $progressFile -Encoding unicode
        $progressBarRun.Maximum = $progress[0]
        $progressBarRun.Value   = $progress[1]
        $form.Refresh()
    }
    elseif ($status -eq 3)  # 3:中断要求
    {
        $timer.Enabled = $False
        $comboBoxSrc.Enabled = $True
        $comboBoxDst.Enabled = $True
        $comboBoxFcb.Enabled = $True
        $buttonOpenSrc.Enabled = $True
        $buttonOpenDst.Enabled = $True
        $buttonOpenFcb.Enabled = $True
        $buttonStop.Enabled = $False
        $buttonRun.Enabled  = $True
        $progressBarRun.Maximum = 0
        $progressBarRun.Value   = 0
        $form.Refresh()
        Set-Content -Path $statusFile -Value "0000" -Encoding ascii # 0:NOP
    }
}

Set-Content -Path $statusFile -Value "0000" -Encoding ascii # 0:NOP
$Font = New-Object System.Drawing.Font("ＭＳ ゴシック",12)

$form.Text = "FCBTool"
$form.Size = New-Object System.Drawing.Size(520,300)
$form.MinimumSize = New-Object System.Drawing.Size(520,300)
$form.StartPosition = "CenterScreen"
$form.font = $Font

$comboBoxSrc.Location = New-Object System.Drawing.Point(100,10)
$comboBoxSrc.size = New-Object System.Drawing.Size(350,30)
$comboBoxSrc.Anchor = 1 + 4 + 8 # 1:Top + 4:Left + 8:Right
$comboBoxSrc.DropDownStyle = "DropDown"
$comboBoxSrc.FlatStyle = "standard"
$comboBoxSrc.font = $Font
$comboBoxSrc.TabIndex = 0

$comboBoxHeight = $comboBoxSrc.Size.Height

$labelSrc = New-Object System.Windows.Forms.Label
$labelSrc.Location = New-Object System.Drawing.Point(10,10)
$labelSrc.Size = New-Object System.Drawing.Size(90,$comboBoxHeight)
$labelSrc.Text = "Source"
$labelSrc.TextAlign = 16 # 16:MiddleLeft
$form.Controls.Add($labelSrc)

$buttonOpenSrc.Location = New-Object System.Drawing.Point(450,10)
$buttonOpenSrc.Size = New-Object System.Drawing.Size(40,$comboBoxHeight)
$buttonOpenSrc.Anchor = 1 + 8 # 1:Top + 8:Right
$buttonOpenSrc.Text = "..."
$buttonOpenSrc.Add_Click({param($s,$e) buttonOpenSrc_Click $s $e})
$form.Controls.Add($buttonOpenSrc)

$comboBoxDst.Location = New-Object System.Drawing.Point(100,40)
$comboBoxDst.size = New-Object System.Drawing.Size(350,30)
$comboBoxDst.Anchor = 1 + 4 + 8 # 1:Top + 4:Left + 8:Right
$comboBoxDst.DropDownStyle = "DropDown"
$comboBoxDst.FlatStyle = "standard"
$comboBoxDst.font = $Font
$comboBoxDst.TabIndex = 0

$labelDst = New-Object System.Windows.Forms.Label
$labelDst.Location = New-Object System.Drawing.Point(10,40)
$labelDst.Size = New-Object System.Drawing.Size(90,$comboBoxHeight)
$labelDst.Text = "Dst"
$labelDst.TextAlign = 16 # 16:MiddleLeft
$form.Controls.Add($labelDst)

$buttonOpenDst.Location = New-Object System.Drawing.Point(450,40)
$buttonOpenDst.Size = New-Object System.Drawing.Size(40,$comboBoxHeight)
$buttonOpenDst.Anchor = 1 + 8 # 1:Top + 8:Right
$buttonOpenDst.Text = "..."
$buttonOpenDst.Add_Click({param($s,$e) buttonOpenDst_Click $s $e})
$form.Controls.Add($buttonOpenDst)

$comboBoxFcb.Location = New-Object System.Drawing.Point(100,70) # Y+=30
$comboBoxFcb.size = New-Object System.Drawing.Size(350,30)
$comboBoxFcb.Anchor = 1 + 4 + 8 # 1:Top + 4:Left + 8:Right
$comboBoxFcb.DropDownStyle = "DropDown"
$comboBoxFcb.FlatStyle = "standard"
$comboBoxFcb.font = $Font
$comboBoxFcb.TabIndex = 0

$labelFcb = New-Object System.Windows.Forms.Label
$labelFcb.Location = New-Object System.Drawing.Point(10,70)
$labelFcb.Size = New-Object System.Drawing.Size(90,$comboBoxHeight)
$labelFcb.Text = "FCB"
$labelFcb.TextAlign = 16 # 16:MiddleLeft
$form.Controls.Add($labelFcb)

$buttonOpenFcb.Location = New-Object System.Drawing.Point(450,70)
$buttonOpenFcb.Size = New-Object System.Drawing.Size(40,$comboBoxHeight)
$buttonOpenFcb.Anchor = 1 + 8 # 1:Top + 8:Right
$buttonOpenFcb.Text = "..."
$buttonOpenFcb.Add_Click({param($s,$e) buttonOpenFcb_Click $s $e})
$form.Controls.Add($buttonOpenFcb)

$radioButtonLeft.Location = New-Object System.Drawing.Point(10,20)
$radioButtonLeft.Size = New-Object System.Drawing.Size(100,30)
$radioButtonLeft.Text = "Left"

$radioButtonRight.Checked = $True
$radioButtonRight.Location = New-Object System.Drawing.Point(130,20)
$radioButtonRight.Size = New-Object System.Drawing.Size(100,30)
$radioButtonRight.Text = "Right"

$groupBoxFcb = New-Object System.Windows.Forms.GroupBox
$groupBoxFcb.Location = New-Object System.Drawing.Point(10,100)
$groupBoxFcb.Size = New-Object System.Drawing.Size(480,60)
$groupBoxFcb.Text = "Opt Selection"
$groupBoxFcb.Controls.Add($radioButtonLeft);
$groupBoxFcb.Controls.Add($radioButtonRight);
$form.Controls.Add($groupBoxFcb)

$progressBarRun.Location = New-Object System.Drawing.Point(10,170)
$progressBarRun.Size = New-Object System.Drawing.Size(480,30)
$progressBarRun.Anchor = 1 + 4 + 8 # 1:Top + 4:Left + 8:Right
$progressBarRun.Minimum = 0
$progressBarRun.Maximum = 0
$progressBarRun.Value   = 0
$form.Controls.Add($progressBarRun)

$buttonStop.Location = New-Object System.Drawing.Point(280,210)
$buttonStop.Size = New-Object System.Drawing.Size(100,30)
$buttonStop.Anchor = 2 + 8 # 2:Bottom + 8:Right
$buttonStop.Text = "Stop"
$buttonStop.Enabled = $False
$buttonStop.Add_Click({param($s,$e) buttonStop_Click $s $e})
$form.Controls.Add($buttonStop)

$buttonRun.Location = New-Object System.Drawing.Point(390,210)
$buttonRun.Size = New-Object System.Drawing.Size(100,30)
$buttonRun.Anchor = 2 + 8 # 2:Bottom + 8:Right
$buttonRun.Text = "Run"
$buttonRun.Add_Click({param($s,$e) buttonRun_Click $s $e})
$form.Controls.Add($buttonRun)

$timer.Enabled = $False
$timer.Interval = 500
$timer.Add_Tick({param($s,$e) timer_Tick $s $e})

$xmlPath = $PSScriptRoot + "\FCBTool.xml"
# xml 読込
# TIPS:StreamReader を使用しないと "他のプロセスが使用中" になる
$sr = [System.IO.StreamReader]::new([System.IO.FileStream]::new($xmlPath,
      [System.IO.FileMode]::Open, [System.IO.FileAccess]::Read,
      [System.IO.FileShare]::ReadWrite + [System.IO.FileShare]::Delete))
$xmlTextReader = New-Object System.Xml.XmlTextReader($sr)
while ($xmlTextReader.Read())
{
    if ($xmlTextReader.NodeType.Equals([System.Xml.XmlNodeType]::Element))
    {
        for ($i=0; $i -lt $aryFind.Count; $i++)
        {
            $aryFind[$i] = $False
            if ($xmlTextReader.Name.Equals($aryName[$i]))
            {
                $aryFind[$i] = $True
            }
        }
    }
    elseif ($xmlTextReader.NodeType.Equals([System.Xml.XmlNodeType]::Text))
    {
        for ($i=0; $i -lt $aryFind.Count; $i++)
        {
            if ($aryFind[$i] -eq $True)
            {
                $aryText[$i] = $xmlTextReader.Value
            }
        }
    }
}
$xmlTextReader.Close()
$xmlTextReader.Dispose()
$sr.Close()
$sr.Dispose()

# xmlを画面へ設定
for ($i = 0; $i -lt $aryName.Count; $i++)
{
    $isTop = $True
    if ($aryName[$i] -eq "opt")
    {
        $opt = [int]$aryText[$i]
        if ($opt -eq 1)
        {
            $radioButtonLeft.Checked = $True
        }
        if ($opt -eq 2)
        {
            $radioButtonRight.Checked = $True
        }
    }
    else
    {
        foreach ($text in $aryText[$i].Split("`n"))
        {
            $text = $text.Trim()
            if ($text.Length -gt 0)
            {
                if ($isTop -eq $True)
                {
                    $isTop = $False
                    $aryComboBox[$i].Text = $text
                }
                [void]$aryComboBox[$i].Items.Add($text)
            }
        }
        $form.Controls.Add($aryComboBox[$i])
    }
}

$form.ShowDialog()

# 画面の入力内容を xml に保存
$crlf = [char]13 + [char]10
for ($i = 0; $i -lt $aryName.Count; $i++)
{
    if ($aryName[$i] -eq "opt")
    {
        $aryText[$i] = ""
        if ($radioButtonLeft.Checked -eq $True)
        {
            $aryText[$i] = "1"
        }
        if ($radioButtonRight.Checked -eq $True)
        {
            $aryText[$i] = "2"
        }
    }
    else
    {
        $aryText[$i] = $crlf
        $text = $aryComboBox[$i].Text.Trim()
        if ($text.Length -gt 0)
        {
            $aryText[$i] += $text + $crlf
        }
        foreach ($item in $aryComboBox[$i].Items)
        {
            $item = $item.Trim()
            if ($item -ne $text)
            {
                $aryText[$i] += $item + $crlf
            }
        }
    }
}

# TIPS:StreamWriter を使用しないと "他のプロセスが使用中" になる
$sw = New-Object System.IO.StreamWriter($xmlPath, $false, [System.Text.Encoding]::UTF8)
$xmlTextWriter = New-Object System.Xml.XmlTextWriter($sw)
$xmlTextWriter.WriteStartDocument()
$xmlTextWriter.WriteWhitespace($crlf)
$xmlTextWriter.WriteStartElement("Settings")
$xmlTextWriter.WriteString($crlf)
for ($i = 0; $i -lt $aryName.Count; $i++)
{
    $xmlTextWriter.WriteElementString($aryName[$i], $aryText[$i])
    $xmlTextWriter.WriteWhitespace($crlf)
}
$xmlTextWriter.WriteEndElement()
$xmlTextWriter.WriteWhitespace($crlf)
$xmlTextWriter.WriteEndDocument()
$xmlTextWriter.Close()
$xmlTextWriter.Dispose()
$sw.Close()
$sw.Dispose()
