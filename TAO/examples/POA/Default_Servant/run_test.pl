eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}'
    & eval 'exec perl -S $0 $argv:q'
    if 0;

# $Id$
# -*- perl -*-

unshift @INC, '../../../../bin';
require ACEutils;

$iorfile = "server.ior";

unlink $iorfile;

$SV = Process::Create ($EXEPREFIX."server$Process::EXE_EXT", " -o $iorfile");

ACE::waitforfile ($iorfile);

$status  = system ($EXEPREFIX."client$Process::EXE_EXT -k $iorfile");

$SV->Kill (); $SV->Wait ();

unlink $iorfile;

exit $status;
