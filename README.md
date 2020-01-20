Обновленная версия Label.

Уровни, отсеки и группы загружаются в LabelStorage.
Метки объектов заружаются в Engine вместе с инициализированным LabelStorage. Метки объектов парсятся в 
Engine и сохраняются в InternalSecurityContext. Пользователь может получить доступ к меткам только 
через SecurityContext, это идентификатор для InternalSecurityContext.