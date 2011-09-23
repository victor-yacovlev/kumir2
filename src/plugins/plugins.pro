TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = kumiranalizer kumircodegenerator kumirnativegenerator kumircompiler kumirbcompiler kumircoderun editor st_funct browser coregui
contains(CONFIG,with_pascal) {
    SUBDIRS += pascalanalizer
}
