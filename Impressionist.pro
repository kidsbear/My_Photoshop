TEMPLATE = subdirs

# Sub-project names
SUBDIRS = \
    sub_glew \
    sub_impr

sub_glew.subdir = Libraries/glew-2.0.0
sub_impr.subdir = Impressionist
sub_impr.depends = sub_glew
