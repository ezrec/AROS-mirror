from db.tasks.parse import *
from db.tasks.format.html import *

tasks = parse( file( '../db/tasks', 'r' ) )
format( tasks, 'out' )
