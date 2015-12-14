#!/usr/bin/perl -w
# /************************************************************************/
# /*                                                                      */
# /* Package:  ACQUA                                                      */
# /* Module:   lago-configs.pl                                            */
# /*                                                                      */
# /************************************************************************/
# /* Authors:  Hernán Asorey                                              */
# /* e-mail:   asoreyh@cab.cnea.gov.ar                                    */
# /* for the LAGO Collaboration, lagoproject.org, lago@lagoproject.org	  */
# /************************************************************************/
# /* Comments: Basic script to configure data acquisition at each site    */
# /*                                                                      */
# /************************************************************************/
# /* 
# Copyright (c) 2013-Today
# The LAGO Project  
# Copyright (c) 2013
# Hernán Asorey
# Lab DPR (CAB-CNEA), Argentina
# Grupo Halley (UIS), Colombia
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# 
#    1. Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
# 
#    2. Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
# NO EVENT SHALL LAB DPR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
# 
# The views and conclusions contained in the software and documentation are
# those of the authors and should not be interpreted as representing
# official policies, either expressed or implied, of Lab DPR.
# 
# */
# /************************************************************************/

# Metadata containers
%configs=();
@parameters=();

# Parameters
# If you need to add more question, just add them in four rows:
# 1) Parameter Id
# 2) Question you need to ask to user
# 3) Number(1), string(0), bool(2) parameter?
# 4) Default value, when possible

push @parameters, 
	# System, 0-3
	"LAGO_DAQ", 
	"LAGO_DAQ environment variable", 
	0,
	"",
	
	# Site, 4-39
	"siteName", 
	"Site Name (don't use white spaces or weird characters)", 
	0,
	"",
	"siteLat", 
	"Site Latitude (+/- dec deg, <0 = South)", 
	1,
	0.,
	"siteLong", 
	"Site Longitude (+/- dec deg, <0 = West)", 
	1,
	0.,
	"siteAlt",
	"Site Altitude (m a.s.l.)",
	1,
	0.,
	"siteRespName",
	"Site Responsible Name (please don't use weird characters)",
	0,
	"",
	"siteRespId",
	"Site Responsible OrcId (typical format: 0000-0000-0000-0000, separate by commas for multiple values)",
	0,
	"0000-0000-0000-0000",
	"siteRespEmail",
	"Site Responsible email (please use a valid and frequently used email)",
	0,
	"",
	"siteInst",
	"Site Institution (as it appears in the institutions data base)",
	0,
	"",
	"siteDetectors",
	"Number of Detectors in this site (1-3 for this electronics)",
	1,
	1,
	
	# Detectors, 40-123
	# Detector 1, 40-67 
	"detector1Name",
	"Name of the detector in ch1",
	0,
	"",
	"detector1Diameter",
	"Diameter of the water volume for the ch1 detector (mm)",
	1,
	0.,
	"detector1Height",
	"Height of the water volume for the ch1 detector (mm)",
	1,
	0.,
	"detector1Material",
	"Detector material (plastic, steel, ...)",
	0,
	"plastic",
	"detector1Coating",
	"Detector internal coating (tyvek, banner, ...)",
	0,
	"Tyvek",
	"detector1PMT",
	"Detector PMT model (Photonis XP1805, Hamamatsu R5912, ...)",
	0,
	"R5912",
	"detector1PMTowner",
	"PMT owner (use LAGO for AugerPMT, Institution instead)",
	0,
	"",
	# Detector 2, 68-95 
	"detector2Name",
	"Name of the detector in ch2",
	0,
	"",
	"detector2Diameter",
	"Diameter of the water volume for the ch2 detector (mm)",
	1,
	0.,
	"detector2Height",
	"Height of the water volume for the ch2 detector (mm)",
	1,
	0.,
	"detector2Material",
	"Detector material (plastic, steel, ...)",
	0,
	"plastic",
	"detector2Coating",
	"Detector internal coating (tyvek, banner, ...)",
	0,
	"Tyvek",
	"detector2PMT",
	"Detector PMT model (Photonis XP1805, Hamamatsu R5912, ...)",
	0,
	"R5912",
	"detector2PMTowner",
	"PMT owner (use LAGO for AugerPMT, Institution instead)",
	0,
	"",
	# Detector 3, 96-123 
	"detector3Name",
	"Name of the detector in ch3",
	0,
	"",
	"detector3Diameter",
	"Diameter of the water volume for the ch3 detector (mm)",
	1,
	0.,
	"detector3Height",
	"Height of the water volume for the ch3 detector (mm)",
	1,
	0.,
	"detector3Material",
	"Detector material (plastic, steel, ...)",
	0,
	"plastic",
	"detector3Coating",
	"Detector internal coating (tyvek, banner, ...)",
	0,
	"Tyvek",
	"detector3PMT",
	"Detector PMT model (Photonis XP1805, Hamamatsu R5912, ...)",
	0,
	"R5912",
	"detector3PMTowner",
	"PMT owner (use LAGO for AugerPMT, Institution instead)",
	0,
	"",
	
	# Calibración, 124-147
	# Calib1 124-131
	"detector1HV",
	"Polarization voltage in ch1 (WARNING: the number you put here is not in V, you have to calibrate the DAQ)",
	1,
	0.,
	"detector1Trigger",
	"Trigger value in ADC counts for ch1",
	1,
	1000,
	
	# Calib2 132-139
	"detector2HV",
	"Polarization voltage in ch2 (WARNING: the number you put here is not in V, you have to calibrate the DAQ)",
	1,
	0.,
	"detector2Trigger",
	"Trigger value in ADC counts for ch2",
	1,
	1000,
	
	# Calib2 140-147
	"detector3HV",
	"Polarization voltage in ch3 (WARNING: the number you put here is not in V, you have to calibrate the DAQ)",
	1,
	0.,
	"detector3Trigger",
	"Trigger value in ADC counts for ch3",
	1,
	1000,
	
	# DAQ 148-167
	"hasUSB",
	"If yes, files will be placed into a usb stick. Otherwise it will be locate in the run directory (y/n)",
	2,
	'y',
	"devUSB",
	"Device for the usb, usually sdXN, but please check to avoid data loss",
	0,
	"sdb1",
	"usb",
	"Where the compressed data and processed files will be located (complete path)",
	0,
	"",
	"fpgaGates",
	"Number of gates of Nexys2 fpga (500/1200)",
	1,
	500,
	"analyzeRaw",
	"Do analyze raw data using L1 raw.cc from ANNA module (y/n)?",
	2,
	"n",
	
	# SENSORS 168-193
	"hasGPS",
	"Is GPS module installed (y/n)",
	2,
	"y",
	"GPSmodel",
	"GPS model (specify all features in a single line)",
	0,
	"",
	"internalGPS",
	"Is GPS connected directly to the FPGA",
	2,
	"y",
	"hasPT",
	"Is PT module installed (y/n)",
	2,
	"y",
	"PTmodel",
	"PT sensor model (specify all features in a single line)",
	0,
	"",
	"internalPT",
	"Is PT sensor connected directly to the FPGA",
	2,
	"y",
	"racimo",
	"Is this a RACIMO station",
	2,
	"n";

sub get {
	my $question = $_[0];
	my $default = (defined($_[1]) ? $_[1] : 0);
	my $param = $_[2];
	print "$question?\n<$param $default>: ";
	chomp (my $tmp = <>);
	if ($tmp eq "") {
	  $tmp=$default;
	}
	return $tmp;
}

sub cmd {
	my $command = $_[0];
	unless (defined($command)) {
		$command = "clear";
	}
	return system ("$command");
}

sub read_configs {
	my $file = $_[0];
	unless (defined($file)) {
		$file = "lago-configs";
	}
	open my $fh, "< $file" or die ("# ERROR\tCould not open $file: $!\n");
	while (<$fh>) {
		chomp;
		next if /^#/;
		next if /^$/;
		s/"//g;
		($p,$v) = split('=',$_);
		if ($v eq 'true') {
			$v = 'y';
		} elsif ($v eq 'false') {
			$v = 'n';
		}
		$configs{"$p"}=$v;
	}
	print "# SUCCESS\tPrevious configuration file read\n";
}

sub ask {
	my $start = $_[0];
	my $end = $_[1];
	my $block = $_[2];
	my $blockN = $_[3];
	my $answer;
	print "\n$block\n";
	print $fh "\n$block\n";
	for ($i=$start; $i <= $end; $i+=4) {
		my $parameter=$parameters[$i];
		my $question=$parameters[$i+1];
		my $type=$parameters[$i+2];
		my $default=$parameters[$i+3];
		if (defined($configs{"$parameter"})) {
			$default=$configs{"$parameter"};
		}
		my $qnum=$i/4;
		if ($iCalib && $blockN != 2) {
			$answer=$default;
		} else {
			$answer=get("$qnum. $question", $default, "$parameter");
		}
		if ($type == 1) {
			$configs{"$parameter"}=1. * $answer;
		} elsif ($type == 2) {
			if ($answer eq 'y') {
				$configs{"$parameter"}="true";
			} else {
				$configs{"$parameter"}="false";
			}
		} else {
			$configs{"$parameter"}="\"" . $answer . "\"";
		}
		print $fh "$parameter=$configs{$parameter}\n";
	}
}

# Default options
$iHelp = 0;
$iCalib = 0;

while ($_ = $ARGV[0]) {
	last unless /^-/;
	shift;
	# options
	# if (/-l$/i) {
	#	$cluster++;
	#	$clsname = $ARGV[0];
	#	shift;
	# }
  # modifiers
	if (/-\?$/i) {
		$iHelp++;
	}
	if (/-c$/i) {
		$iCalib++;
	}
}

$usage="
	The LAGO Project, http://lagoproject.org
	The LAGO ACQUA aquisition suite\n
	$0
	A simple script to configure LAGO ACQUA acquisition
	(c) 2015-Today, The LAGO Project, lago\@lagoproject.org
	(Original (c) 2015, H. Asorey, asoreyh\@cab.cnea.gov.ar)
	If you enjoy it, please send us an email\n
	Usage: $0 [OPTION] ... \n
	Asks questions, produce the current configuration 'lago-configs' file\n
		-c		Ask only for detector calibration parameters
		-?		Shows this help and ends
	\n
	Examples:
		$0 
	
	LAGO ACQUA, http://wiki.lagoproject.org/index.php?title=ACQUA";

if ($iHelp) {
	die "$usage\n";
}

if ($iCalib) {
	print "# WARNING\t\tOnly calibration parameters will be asked.\n";
}
# Let's start

# Check if LAGO environment is defined
$now = gmtime();
$home=$ENV{'HOME'};
$pwd=$ENV{'PWD'};
unless (defined($ENV{"$parameters[0]"})) {
	print "# WARNING\t$parameters[0] environment variable is not defined.\n\t\tProbably this is a first installation of LAGO ACQUA in this\n\t\tsystem. I will include a line into your .bashrc (assuming\n\t\tyou are using bash. If not just add the contents of .bashrc\n\t\tinto your environments file). The original .bashrc will be\n\t\trenamed to .bashrc-lago.bak.\n";
	$tmp=get("Please confirm (y/n)",'y',"$parameters[0]");
	$iCalib=0;
	die "# STATUS\tConfiguration not changed. Bye\n" unless ($tmp eq 'y');
	$ENV{"$parameters[0]"}=$pwd;
	$configs{"$parameters[0]"}=$pwd;
	open ($fh, "> tmp") or die "# ERROR\tCan't write files here!: $!\n";
	print $fh "\n#\n# Changes added by the LAGO ACQUA suite on $now UTC\n#\n";
	print $fh "export $parameters[0]='$configs{$parameters[0]}'\n\n";
	print $fh "export PATH=\"$configs{$parameters[0]}:\$PATH\"\n\n";
	close($fh);
	cmd("cp $home/.bashrc $home/.bashrc-lago.bak");
	cmd("cat tmp >> $home/.bashrc")	;
	cmd("rm tmp");
	print "# SUCCESS\tLAGO environment variable is set and .bashrc was changed\n\n";
} else { # ok. is defined, but is the same as current directory?
	unless ($ENV{$parameters[0]} eq $pwd) {
		$tmp=get("# WARNING\tIt looks like you are not in the standard LAGO ACQUA location:\n\t\tStandard: $ENV{$parameters[0]}\n\t\tCurrent: $pwd\nDo you want to change it here (I will change .bashrc also)? (y/n)", 'n', "NEWENV");
		if ($tmp eq 'y') {
			$ENV{"$parameters[0]"}=$pwd;
			$configs{"$parameters[0]"}=$pwd;
			open ($fh, "> tmp") or die "# ERROR\tCan't write files here!: $!\n";
			print $fh "\n#\n# Changes added by the LAGO ACQUA suite on $now UTC\n#\n";
			print $fh "export $parameters[0]='$pwd'\n\n";
			close($fh);
			cmd("cp $home/.bashrc $home/.bashrc-lago.bak");
			cmd("cat tmp >> $home/.bashrc")	;
			cmd("rm tmp");
			$iCalib=0;
			print "# SUCCESS\tLAGO environment variable is set and .bashrc was changed\n\n";
		} else {
			print "# STATUS\tNothing change, but I don't understand what do you want to do then.\n";
		}
	}
}

if (-e "lago-configs") {
	$tmp=get("# WARNING\tIt looks like you will change the DAQ configuration.\nPlease confirm to continue (y/n)", 'y', "NEWCONFIG");
	die "# STATUS\tConfiguration not changed. Bye\n" unless ($tmp eq 'y');
	read_configs();
	die "# ERROR\tSomething was wrong creating new 'lago-configs' file. Nothing change.\nBye\n" unless not (cmd('mkdir -p lago-old-configs; mv lago-configs lago-old-configs/lago-configs_$(date -u +%Y%m%d_%H%M%S)'));
} else {
	$tmp=get("# WARNING\tI can't find a previous 'lago-configs' file.\n\t\tThis could mean that this is the first time you will run\n\t\tthe LAGO ACQUA BRC DAQ in this system, or that you are not\n\t\tin the correct directory. If you continue, a new lago-configs\n\t\tfile will be created here and the current path will be assumed\n\t\tas the LAGO DAQ main directory.\nDo you want to continue (y/n)",'n',"NEWCONFIG");
	die "\n# STATUS\tCheck if you are in the correct directory. Conf not changed.\nBye\n" unless ($tmp eq 'y');
	$iCalib=0;
}

# new lago-configs
open ($fh, "> lago-configs-tmp") or die "# ERROR\tCan't open lago-configs file for writing: $!\n";
$now = gmtime();
print $fh "#########################################################
# LAGO AQCUA DAQ CONFIGURATION FILE                     #
# (c) The LAGO Project - lago\@lagoproject.org           #
# 2015 - Today                                          #
#                                                       #
# PLEASE DON'T MODIFY THIS FILE BY HAND.                #
# IF YOU NEED TO CHANGE SOME CONFIGURATION PARAMETERS   #
# PLEASE USE                                            #
#                                                       #
# ./lago-config.pl                                      #
#                                                       #
#########################################################

# This file was created on $now UTC
";
# system
$configs{"$parameters[0]"}=$ENV{"$parameters[0]"};
print $fh "\n# BLOCK\t\tSYSTEM\n";
print $fh "$parameters[0]=\"$configs{$parameters[0]}\"\n";

# Ask the questions....
# blocks
$blockN=0; # SITE
$block="# BLOCK\t\tSITE PARAMETERS";

$blockN=1; # DETECTORS
ask(4,39,$block,$blockN);
$block="# BLOCK\t\tDETECTOR PARAMETERS: 1";
ask(40,67,$block,$blockN);
if ($configs{"siteDetectors"} > 1) {
	$block="# BLOCK\t\tDETECTOR PARAMETERS: 2";
	ask(68,95,$block,$blockN);
}
if ($configs{"siteDetectors"} > 2) {
	$block="# BLOCK\t\tDETECTOR PARAMETERS: 3";
	ask(96,123,$block,$blockN);
}

$blockN=2; # CALIBRATION
$block="# BLOCK\t\tCALIBRATION PARAMETERS: 1";
ask(124,131,$block,$blockN);
if ($configs{"siteDetectors"} > 1) {
	$block="# BLOCK\t\tCALIBRATION PARAMETERS: 2";
	ask(132,139,$block,$blockN);
}
if ($configs{"siteDetectors"} > 2) {
	$block="# BLOCK\t\tCALIBRATION PARAMETERS: 3";
	ask(140,147,$block,$blockN);
}

$blockN=3; # DAQ
$block="# BLOCK\t\tACQUISITION SYSTEM";
ask(148,167,$block,$blockN);

$blockN=4; # SENSORS
$block="# BLOCK\t\tSENSORS";
ask(168,193,$block,$blockN);

# not asking values (backward compatibility)
$now=time;
print $fh "work=\"$configs{$parameters[0]}\"\n";
print $fh "user=\"$ENV{'USER'}\"\n";
print $fh "configTime=$now\n";
print "# SUCCESS\tDone. Writing the new lago-configs file\n";
cmd("mv lago-configs-tmp lago-configs");

# now, modify crontab.run
$bash=`which bash`;
chomp($bash);
open($fh, "> crontab.run") or die "# ERROR:\t\tCan't open crontab.run: $!\n";
print $fh "# Edit this file to introduce tasks to be run by cron.
# # 
# # Each task to run has to be defined through a single line
# # indicating with different fields when the task will be run
# # and what command to run for the task
# # 
# # To define the time you can provide concrete values for
# # minute (m), hour (h), day of month (dom), month (mon),
# # and day of week (dow) or use '*' in these fields (for 'any').# 
# # Notice that tasks will be started based on the cron's system
# # daemon's notion of time and timezones.
# # 
# # Output of the crontab jobs (including errors) is sent through
# # email to the user the crontab file belongs to (unless redirected).
# # 
# # For example, you can run a backup of all your user accounts
# # at 5 a.m every week with:
# # 0 5 * * 1 tar -zcf /var/backups/home.tgz /home/
# # 
# # For more information see the manual pages of crontab(5) and cron(8)
# # 
# # m h  dom mon dow   command
*/5 * * * * $bash -ic $configs{$parameters[0]}/lago-start.sh > $configs{$parameters[0]}/lago-daq.log 2>&1
10 * * * * $bash -ic $configs{$parameters[0]}/lago-proc.sh 
# eventually some rsync.sh to transfer files to the repository
";
close($fh);

# and crontab.stop
open($fh, "> crontab.stop") or die "# ERROR:\t\tCan't open crontab.stop: $!\n";
print $fh "# Edit this file to introduce tasks to be run by cron.
# # 
# # Each task to run has to be defined through a single line
# # indicating with different fields when the task will be run
# # and what command to run for the task
# # 
# # To define the time you can provide concrete values for
# # minute (m), hour (h), day of month (dom), month (mon),
# # and day of week (dow) or use '*' in these fields (for 'any').# 
# # Notice that tasks will be started based on the cron's system
# # daemon's notion of time and timezones.
# # 
# # Output of the crontab jobs (including errors) is sent through
# # email to the user the crontab file belongs to (unless redirected).
# # 
# # For example, you can run a backup of all your user accounts
# # at 5 a.m every week with:
# # 0 5 * * 1 tar -zcf /var/backups/home.tgz /home/
# # 
# # For more information see the manual pages of crontab(5) and cron(8)
# # 
# # m h  dom mon dow   command
";
close($fh);

print "# SUCCESS\tNew crontab.run created.\n";
print "# SUCCESS\tEverything was fine. Enjoy.\n";
print "######################################################################\n";
print "# Now, we are ready to start the acquisition                         #\n";
print "# Please verify lago-configs content to be sure everything is right  #\n";
print "# cat lago-configs                                                   #\n";
print "# BEFORE TO CONTINUE, PLEASE:  source $home/.bashrc             #\n";
print "# and then:                    lago-start.sh                         #\n";
print "######################################################################\n";
