# interface generated by SpecTcl version 1.1 from H:/asterisk/iaxclient/simpleclient/tkphone/pref.ui
#   root     is the parent window for this user interface

proc pref_ui {root args} {

	# this treats "." as a special case

	if {$root == "."} {
	    set base ""
	} else {
	    set base $root
	}
    
	frame $base.frame#1

	label $base.label#6 \
		-text Settings

	label $base.label#1 \
		-anchor e \
		-text {IAX server}

	entry $base.entry#1 \
		-textvariable input_server

	label $base.label#2 \
		-anchor e \
		-text {authentication name}

	entry $base.entry#2 \
		-textvariable input_user \
		-width 8

	label $base.label#3 \
		-anchor e \
		-text {authentication password}

	entry $base.entry#3 \
		-show * \
		-textvariable input_pass \
		-width 8

	label $base.label#4 \
		-anchor e \
		-text {phone extension}

	entry $base.entry#4 \
		-textvariable input_ext \
		-width 5

	button $base.button#4 \
		-command pref_ok \
		-text ok

	button $base.button#5 \
		-command pref_cancel \
		-text cancel


	# Geometry management

	grid $base.frame#1 -in $root	-row 6 -column 1  \
		-columnspan 2
	grid $base.label#6 -in $root	-row 1 -column 1  \
		-columnspan 2
	grid $base.label#1 -in $root	-row 2 -column 1  \
		-sticky ew
	grid $base.entry#1 -in $root	-row 2 -column 2 
	grid $base.label#2 -in $root	-row 3 -column 1  \
		-sticky ew
	grid $base.entry#2 -in $root	-row 3 -column 2  \
		-sticky w
	grid $base.label#3 -in $root	-row 4 -column 1  \
		-sticky ew
	grid $base.entry#3 -in $root	-row 4 -column 2  \
		-sticky w
	grid $base.label#4 -in $root	-row 5 -column 1  \
		-sticky ew
	grid $base.entry#4 -in $root	-row 5 -column 2  \
		-sticky w
	grid $base.button#4 -in $base.frame#1	-row 1 -column 1 
	grid $base.button#5 -in $base.frame#1	-row 1 -column 2 

	# Resize behavior management

	grid rowconfigure $root 1 -weight 0 -minsize 30
	grid rowconfigure $root 2 -weight 0 -minsize 9
	grid rowconfigure $root 3 -weight 0 -minsize 5
	grid rowconfigure $root 4 -weight 0 -minsize 6
	grid rowconfigure $root 5 -weight 0 -minsize 11
	grid rowconfigure $root 6 -weight 0 -minsize 30
	grid columnconfigure $root 1 -weight 0 -minsize 30
	grid columnconfigure $root 2 -weight 0 -minsize 30

	grid rowconfigure $base.frame#1 1 -weight 0 -minsize 30
	grid columnconfigure $base.frame#1 1 -weight 0 -minsize 30
	grid columnconfigure $base.frame#1 2 -weight 0 -minsize 30
# additional interface code
# end additional interface code

}


# Allow interface to be run "stand-alone" for testing

catch {
    if [info exists embed_args] {
	# we are running in the plugin
	pref_ui .
    } else {
	# we are running in stand-alone mode
	if {$argv0 == [info script]} {
	    wm title . "Testing pref_ui"
	    pref_ui .
	}
    }
}
