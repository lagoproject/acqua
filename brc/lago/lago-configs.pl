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

use Switch;

# Metadata containers
%configs=();
@parameters=();
@parameter_questions=();

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
		($p,$v) = split('=',$_);
		$configs{"$p"}=$v;
	}
	print "# SUCCESS\tPrevious configuration file read\n";
}
# Default options
$help = 0;

while ($_ = $ARGV[0]) {
  last unless /^-/;
  shift;
  # options
#  if (/-l$/i) {
#    $cluster++;
#    $clsname = $ARGV[0];
#    shift;
#  }
  # modifiers 
  if (/-\?$/i) {
    $help++;
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
		-?		Shows this help and ends
	\n
	Examples:
		$0 
	
	LAGO ACQUA, http://wiki.lagoproject.org/index.php?title=ACQUA";

if ($help) {
  die "$usage\n";
}

# Let's start
push @parameters, "LAGO_DAQ"; 
push @parameter_questions, "LAGO_DAQ environment variable";

# Check if LAGO environment is defined
$now = gmtime();
$home=$ENV{'HOME'};
$pwd=$ENV{'PWD'};
unless (defined($ENV{"$parameters[0]"})) {
	print "# WARNING\t$parameters[0] environment variable is not defined.\n\t\tProbably this is a first installation of LAGO ACQUA in this\n\t\tsystem. I will include a line into your .bashrc (assuming\n\t\tyou are using bash. If not just add the contents of .bashrc\n\t\tinto your environments file). The original .bashrc will be\n\t\trenamed to .bashrc-lago.bak.\n";
	$tmp=get("Please confirm (y/n)",'y',"$parameters[0]");
	die "# STATUS\tConfiguration not changed. Bye\n" unless ($tmp eq 'y');
	$ENV{"$parameters[0]"}=$pwd;
	$configs{"$parameters[0]"}=$pwd;
	open ($fh, "> tmp") or die "# ERROR\tCan't write files here!: $!\n";
	print $fh "\n#\n# Changes added by the LAGO ACQUA suite on $now UTC\n#\n";
	print $fh "export $parameters[0]='$pwd'\n\n";
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
	$tmp=get("# WARNING\tI can't find a previous 'lago-configs' file.\nThis could mean that this is the first time you will run the LAGO ACQUA BRC DAQ\nin this system, or that you are not in the correct directory. If you continue, a new lago-configs file will be created here and the current path will be assumed as the LAGO DAQ main directory.\n\nDo you want to continue (y/n)",'n',"NEWCONFIG");
	die "\n# STATUS\tCheck if you are in the correct directory. Configuration not changed.\nBye\n" unless ($tmp eq 'y');
}

# Parameters
print "\n# BLOCK\t\tSITE PARAMETERS\n";
push @parameters, 
"siteName", 
"siteLat", 
"siteLong", 
"siteAlt",
"siteRespName",
"siteRespId",
"siteRespEmail",
"siteInst",
"siteDetectors",
"detector1Name",
"detector1Diameter",
"detector1Height",
"detector1HV",
"detector1Trigger";

push @parameter_questions, 
"Site Name", 
"Site Latitude (+/- dec deg, <0 = South)", 
"Site Longitude (+/- dec deg, <0 = West)", 
"Site Altitude (m a.s.l.)",
"Site Responsible Name",
"Site Responsible OrcId",
"Site Responsible email",
"Site Institution",
"Number of Detectors in this site";



# Ask the questions....

foreach $i (1 .. @parameters-1) {
	$parameter=$parameters[$i];
	$parameter_question=$parameter_questions[$i];
	$configs{"$parameter"}=get("$i. $parameter_question",$configs{"$parameter"},"$parameter");
}
cmd();

# ... and write the answers...

print "# SUCCESS\tDone. Writing the new lago-configs file\n";
open ($fh, "> lago-configs") or die "# ERROR\tCan't open lago-configs file for writing: $!\n";
$now = gmtime();
print $fh "#########################################################
# LAGO AQCUA DAQ CONFIGURATION FILE						#
# (c) The LAGO Project - lago\@lagoproject.org			#
# 2015 - Today											#
#														#
# PLEASE DON'T MODIFY THIS FILE BY HAND.				#
# IF YOU NEED TO CHANGE SOME CONFIGURATION PARAMETERS	#
# PLEASE USE											#
#														#
# ./lago-config.pl										#
#														#
#########################################################

# This file was created on $now UTC
";

foreach $key (sort keys %configs) {
	print $fh "$key=$configs{$key}\n";
}
print "# SUCCESS\tAll done. Configuring and launching acquisition...\n";
# do something here
print "# SUCCESS\tDone. Acquisition is running on screen:\n";
print "# SUCCESS\tEverything was fine. Enjoy.\n";
