require 'dl/import'
require 'dl/struct'

module CopyData
	extend DL::Importable
	dlload 'user32'
	typealias "WPARAM", "UINT"
  	typealias "LPARAM", "UINT"
	WM_COPYDATA = 0x004A
	CopyData = struct [ 
		'ULONG dwData',
		'DWORD cbData',
		'PVOID lpData', 
	] 
	extern 'UINT SendMessage(HWND, UINT, WPARAM, LPARAM)'
end

begin
hwnd = open(ARGV[0].to_s + ".hwnd").gets.to_i;
rescue
exit
else
end

storage = ARGV[1].to_s;
num     = ARGV[2].to_i;
label   = "";

f = open(storage);
while f.lineno < num && f.gets
  if $_ =~ /^(\*[^|]+)/
	label = $1
  end
end

msg = "jump\t" + storage + "\t" + label;
if (ARGV[3] == "next") then msg += "\tnext" end

cd = CopyData::CopyData.malloc
cd.dwData = 0
cd.cbData = msg.size
cd.lpData = msg
CopyData::sendMessage(hwnd, CopyData::WM_COPYDATA, 0, cd.to_ptr.to_i)
