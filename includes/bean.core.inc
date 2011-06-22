<?php

/**
 * The Bean entity class
 */
class Bean extends Entity {
  public $label;
  public $title;
  public $type;
  public $data;
  protected $plugin;

  /**
   * Get Plugin info
   */
  public function getInfo($key = NULL) {
    return $this->plugin->getInfo($key);
  }

  public function defaultLabel() {
    return $this->label;
  }

  public function __construct($values = array()) {
    parent::__construct($values, 'bean');
    // Load the plugin info
    // NOTE: When this is caalled as part of entity_load, $values seems to be
    // empty, which makes it impossible to load the plugin and field data.
    // in this case, we don't do this now and rely on the caller of entity_load to
    // do it manually.
    if (!empty($this->type)) {
      $this->setPlugin();
      $this->setFields();
    }
  }

  /**
   * Load and set the plugin info.
   * FIXME: This should probably be be private, but we need to be able to call it
   * externally for now (see note in __construct).
   */
  public function setPlugin() {
    $this->plugin = bean_load_plugin_class($this->type);
  }
  
  /**
   * Set the fields from the defaults and plugin
   * FIXME: This should probably be be private, but we need to be able to call it
   * externally for now (see note in __construct).
   */
  public function setFields() {
    // NOTE: When setFields is caled externally $this->data is already unserializd.
    $values = is_array($this->data)? $this->data : unserialize($this->data);
    foreach ($this->plugin->values() as $field => $default) {
      $this->$field = isset($values[$field]) ? $values[$field] : $default;
    }
  }

  /**
   * Override this in order to implement a custom default URI and specify
   * 'entity_class_uri' as 'uri callback' hook_entity_info().
   */
  protected function defaultUri() {
    return array('path' => 'admin/structure/bean/block/' . $this->identifier());
  }

  /**
   * Get the plugin form
   */
  public function getForm() {
    return $this->plugin->form($this);
  }

  /**
   * Validate the plugin form
   */
  public function validate($values) {
    $this->plugin->validate($values);
  }

  /**
   * Edit URL
   */
  public function editURL() {
    return 'block/' . $this->identifier() . '/edit';
  }

  /**
   * View URL
   */
  public function viewURL() {
    return 'block/' . $this->identifier();
  }

  /**
   * Type name
   */
  public function typeName() {
    return $this->getInfo('label');
  }
  
  /**
   * Set the values from a plugin
   */
  public function setValues($values) {
    $this->data = array();
    foreach ($this->plugin->values() as $field => $value) {
      $this->data[$field] = $values[$field];
    }
  }

  /**
   * Generate an array for rendering the entity.
   *
   * @see entity_view()
   */
  public function view($view_mode = 'full', $langcode = NULL) {
    $content = parent::view();

    return $this->plugin->view($this, $content, $view_mode, $langcode);
  }

  /**
   * Override the save to add clearing of caches
   */
  public function save() {
    $return = parent::save();
    cache_clear_all();
    return $return;
  }

  /**
   * Override the delete to remove the fields and blocks
   */
  public function delete() {
    // Delete the field values
    field_attach_delete('bean', $this);

    // Delete any blocks
    // @see block_custom_block_delete_submit()
    db_delete('block')
      ->condition('module', 'bean')
      ->condition('delta', $this->bid)
      ->execute();
    db_delete('block_role')
      ->condition('module', 'bean')
      ->condition('delta', $this->bid)
      ->execute();

    // @see node_form_block_custom_block_delete_submit()
    db_delete('block_node_type')
      ->condition('module', 'bean')
      ->condition('delta', $this->bid)
      ->execute();

    parent::delete();
  }
}