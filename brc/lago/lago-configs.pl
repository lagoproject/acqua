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
@paramters=();
@paramter_questions=();

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
		$config{"$p"}=$v;
	}
	print "# SUCCESS\tPrevious configuration file read\n";
}
# Default options
$help = 0;
$batch = 0;

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

##### Let's start

cmd();
if (-e "lago-configs") {
	$tmp=get("It looks like you will change the DAQ configuration.\nPlease confirm to continue (y/n)", 'y', "NEWCONFIG");
	die "# STATUS\tConfiguration not changed. Bye\n" unless ($tmp eq 'y');
	read_configs();
	die "# ERROR\tSomething was wrong creating new 'lago-configs' file. Nothing change.\nBye\n" unless not (cmd('mkdir -p lago-old-configs; mv lago-configs lago-old-configs/lago-configs_$(date -u +%Y%m%d_%H%M%S)'));
} else {
	$tmp=get("# WARNING\tI can't find a previous 'lago-configs' file.\nThis could mean that this is the first time you will run the LAGO ACQUA BRC DAQ\nin this system, or that you are not in the correct directory. If you continue, a new lago-configs file will be created here and the current path will be assumed as the LAGO DAQ main directory.\n\nDo you want to continue (y/n)",'n',"NEWCONFIG");
	die "\n# STATUS\tCheck if you are in the correct directory. Configuration not changed.\nBye\n" unless ($tmp eq 'y');
}

print "\n# BLOCK\t\tSITE PARAMETERS\n";
push @parameters, "sitename", "sitelat", "sitelong", "sitealt";
push @parameter_questions, "Site Name", "Site Latitude (+/- dec deg, <0 = South)", "Site Longitude (+/- dec deg, <0 = West)", "Site Altitude (m a.s.l.)";
foreach $i (0 .. @parameters-1) {
	$parameter=$parameters[$i];
	$parameter_question=$parameter_questions[$i];
	$j=$i+1;
	$config{"$parameter"}=get("$j. $parameter_question",$config{"$parameter"},"$parameter");
}
cmd();

print "# SUCCESS\tDone. Writing the new lago-configs file\n";
open ($fh, "> lago-configs") or die "# ERROR\tCan't open lago-configs file for writing: $!\n";
foreach $key (sort keys %config) {
	print $fh "$key=$config{$key}\n";
}
print "# SUCCESS\tAll done. Configuring and launching acquisition...\n";
# do something here
print "# SUCCESS\tDone. Acquisition is running on screen:\n";
print "# SUCCESS\tEverything was fine. Enjoy.\n";
